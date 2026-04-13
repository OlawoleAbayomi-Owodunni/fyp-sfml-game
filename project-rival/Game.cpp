#include "Game.h"
#include "InputManager.h"

#include "GruntEnemy.h"
#include "TurretEnemy.h"

#include "CombatRoom.h"
#include "PortalRoom.h"
#include "SpawnRoom.h"

#include "RoomDoorUtils.h"

#include <iostream>
#include <sstream>
#include <string>

/**
 * @file Game.cpp
 * @brief Implements the `Game` class runtime loop and supporting gameplay systems.
 *
 * This translation unit contains the main per-frame orchestration for gameplay,
 * including input handling, simulation, rendering, and higher-level system
 * coordination (hub world, dungeon floors, quests, and LLM jobs).
 */

/**
 * @brief Target fixed update rate used by `Game::run()`.
 */
static double const FPS{ 60.0f };

namespace
{
 /**
	 * @brief Extracts a line value that follows a tag within a larger text blob.
	 *
	 * Searches for @p tag in @p text and returns the substring up to the next newline.
	 * Leading/trailing whitespace is trimmed from the returned value.
	 *
	 * @param text Full response text.
	 * @param tag Tag prefix to search for (e.g. `"TITLE:"`).
	 * @return Extracted value or an empty string when the tag is not present.
	 */
	std::string extractTaggedLineValue(const std::string& text, const std::string& tag)
	{
		const std::size_t tagPos = text.find(tag);
		if (tagPos == std::string::npos)
			return "";

		const std::size_t valueStart = tagPos + tag.size();
		std::size_t valueEnd = text.find('\n', valueStart);
		if (valueEnd == std::string::npos)
			valueEnd = text.size();

		std::string value = text.substr(valueStart, valueEnd - valueStart);
		while (!value.empty() && (value.front() == ' ' || value.front() == '\t'))
			value.erase(value.begin());
		while (!value.empty() && (value.back() == ' ' || value.back() == '\t' || value.back() == '\r'))
			value.pop_back();

		return value;
	}

    /**
	 * @brief Trims leading and trailing whitespace characters.
	 * @param text Input string to trim.
	 * @return Trimmed copy of the input string.
	 */
	std::string trimWhitespace(std::string text)
	{
		while (!text.empty() && (text.front() == ' ' || text.front() == '\t' || text.front() == '\n' || text.front() == '\r'))
			text.erase(text.begin());
		while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\n' || text.back() == '\r'))
			text.pop_back();
		return text;
	}

   /**
	 * @brief Extracts the first line of text and trims it.
	 * @param text Input string.
	 * @return First line with surrounding whitespace removed.
	 */
	std::string extractFirstLine(const std::string& text)
	{
		std::size_t end = text.find('\n');
		if (end == std::string::npos)
			end = text.size();
		return trimWhitespace(text.substr(0, end));
	}

  /**
	 * @brief Removes a leading tag prefix when present.
	 * @param text Input string.
	 * @param tag Prefix to strip when it appears at the start of the string.
	 * @return Input string with the prefix removed if it was present.
	 */
	std::string stripTaggedPrefix(const std::string& text, const std::string& tag)
	{
		std::string value = trimWhitespace(text);
		if (value.rfind(tag, 0) == 0)
			value = trimWhitespace(value.substr(tag.size()));
		return value;
	}

 /**
	 * @brief Extracts lore text payload from an LLM response.
	 *
	 * If a `LORE:` tag exists, returns everything after the tag, otherwise returns
	 * the full input trimmed.
	 *
	 * @param text Full response text.
	 * @return Lore payload suitable for further formatting.
	 */
	std::string extractLorePayload(const std::string& text)
	{
		const std::size_t tagPos = text.find("LORE:");
		if (tagPos != std::string::npos)
			return trimWhitespace(text.substr(tagPos + 5));

		return trimWhitespace(text);
	}

  /**
	 * @brief Formats lore text into exactly two lines when possible.
	 *
	 * The formatter prefers existing newline separation; otherwise it attempts to
	 * split by sentence terminators.
	 *
	 * @param rawLore Raw lore payload extracted from the LLM.
	 * @return A two-line string when possible; otherwise a trimmed single string.
	 */
	std::string formatLoreTwoLines(const std::string& rawLore)
	{
		std::string lore = trimWhitespace(rawLore);
		if (lore.empty())
			return lore;

		std::vector<std::string> lines;
		std::stringstream ss(lore);
		std::string line;
		while (std::getline(ss, line))
		{
			line = trimWhitespace(line);
			if (!line.empty())
				lines.push_back(line);
		}

		if (lines.size() >= 2)
			return lines[0] + "\n" + lines[1];

		std::vector<std::string> sentences;
		std::string current;
		for (char c : lore)
		{
			current.push_back(c);
			if (c == '.' || c == '!' || c == '?')
			{
				std::string sentence = trimWhitespace(current);
				if (!sentence.empty())
					sentences.push_back(sentence);
				current.clear();
			}
		}

		if (!trimWhitespace(current).empty())
			sentences.push_back(trimWhitespace(current));

		if (sentences.size() >= 2)
			return sentences[0] + "\n" + sentences[1];

		return lore;
	}

    /**
	 * @brief Chooses a short NPC context line used to condition quest metadata prompts.
	 *
	 * This is used to help produce more varied LLM quest titles/lore while keeping
	 * the gameplay requirements (type/targets/reward) unchanged.
	 *
	 * @param quest Quest description the context should match.
	 * @return A short context string tied to an implied hub NPC.
	 */
	std::string getRandomQuestNpcContext(const QuestData& quest)
	{
		if (quest.type == QuestType::KILL)
		{
			static const std::vector<std::string> guntherContexts{
				"Gunther needs the tunnels cleared so black market shipments can move safely again.",
				"Gunther put a bounty out after armed stragglers started extorting his suppliers.",
				"Gunther wants local hostiles reduced before a high-value weapons convoy arrives."
			};
			return guntherContexts[rand() % guntherContexts.size()];
		}

		if (quest.pickupType == PickupType::AMMO)
		{
			static const std::vector<std::string> arnoldContexts{
				"Arnold is reinforcing defenses and needs fresh ammunition caches before nightfall.",
				"Arnold's forge guards are rationing rounds, so he is paying for every recovered ammo pack.",
				"Arnold expects a siege and needs emergency stockpiles delivered from the lower floors."
			};
			return arnoldContexts[rand() % arnoldContexts.size()];
		}

		static const std::vector<std::string> petraContexts{
			"Petra is treating wounded hunters and needs medical packs before the clinic runs dry.",
			"Petra promised sanctuary to injured adventurers and is desperate for more supplies.",
			"Petra's triage station is overloaded, so she is paying for urgent recovery packs."
		};
		return petraContexts[rand() % petraContexts.size()];
	}
}

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode::getDesktopMode(), "FYP: Dungeonish Crawler", sf::State::Fullscreen)
{
	m_llmManager.initAsync("ASSETS/LLM/MODELS/Llama-3.2-1B-Instruct-Q4_K_M.gguf");

	srand(time(NULL));
	init();
}

////////////////////////////////////////////////////////////
/**
 * @brief Initializes runtime state and enters the initial hub world.
 *
 * Sets up rendering/UI resources, debug overlays (when enabled), and initial
 * gameplay state.
 */
void Game::init()
{
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);
	
	if (!m_arialFont.openFromFile("ASSETS/FONTS/ariblk.ttf"))
	{
		std::cout << "Error loading font file";
	}

#ifdef TEST_FPS
	x_updateFPS.setFont(m_arialFont);
	x_updateFPS.setPosition(sf::Vector2f(20, 300));
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_arialFont);
	x_drawFPS.setPosition(sf::Vector2f(20, 350));
x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);
#endif

	m_hud.init(m_arialFont);
	
	m_roomDescriptionText.setFont(m_arialFont);
	m_roomDescriptionText.setCharacterSize(18);
	m_roomDescriptionText.setFillColor(sf::Color::White);
	m_roomDescriptionText.setOutlineThickness(1.f);
	m_roomDescriptionText.setOutlineColor(sf::Color::Black);
	m_roomDescriptionText.setPosition(sf::Vector2f(20, 400));

	#pragma region Menu UI
    m_menuUI.init(m_arialFont, m_window.getDefaultView());
    m_menuUI.setScreen(MenuScreen::MAIN_MENU_SCREEN);
	#pragma endregion
	m_activeShop = HubShopType::NONE_SHOP;
	m_isAtJobBoard = false;

	m_activeNPCId = -1;
	m_isNPCDialogueOpen = false;
	m_selectedResponse = -1;
	m_currentDialogueOptions.clear();

	enterHubWorld();
}

////////////////////////////////////////////////////////////
/**
 * @brief Runs the main fixed-timestep game loop.
 *
 * - Polls window events.
 * - Updates simulation at a fixed timestep (`FPS`).
 * - Renders as often as possible.
 */
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asSeconds()); //60 fps
#ifdef TEST_FPS
			x_secondTime += timePerFrame;
			x_updateFrameCount++;
			if (x_secondTime.asSeconds() > 1)
			{
				std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
				x_updateFPS.setString(updatesPS);
				std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
				x_drawFPS.setString(drawsPS);
				x_updateFrameCount = 0;
				x_drawFrameCount = 0;
				x_secondTime = sf::Time::Zero;
			}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}

////////////////////////////////////////////////////////////
/**
 * @brief Polls OS/window events and forwards them to `Game::processGameEvents()`.
 */
void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }
        processGameEvents(*event);
    }
}


////////////////////////////////////////////////////////////
/**
 * @brief Handles gameplay and UI events.
 *
 * When a menu screen is active, events are routed to `MenuUI`. Otherwise the
 * handler processes gameplay inputs such as room description requests and NPC chat.
 *
 * @param event Event from SFML.
 */
void Game::processGameEvents(const sf::Event& event)
{
#pragma region Menu UI
    if (!m_menuUI.isGameplayScreen())
	{
        m_menuUI.processEvent(event, m_window);
		applyMenuAction(m_menuUI.consumeAction());
		return;
	}
#pragma endregion

	// NPC Typed Input
	if (m_isNPCDialogueOpen && m_activeNPCId >= 0 && m_activeNPCId < m_hubNPCs.size())
	{
		if (const auto* textEntered = event.getIf<sf::Event::TextEntered>())
		{
			const char32_t unicode = textEntered->unicode;
			//Backspace
			if (unicode == 8) {
				if (!m_npcInputBuffer.empty())
					m_npcInputBuffer.pop_back();
			}
			// Printable charachters
			else if (unicode >= 32 && unicode <= 126) {
				m_npcInputBuffer.push_back(static_cast<char>(unicode));
			}
		}

		if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
		{
			// Submit input
          if (keyPressed->scancode == sf::Keyboard::Scancode::Enter && !m_npcInputBuffer.empty())
			{
				const HubNPCInfo& npcInfo = m_hubNPCs[m_activeNPCId].getInfo();
				const std::string prompt = "You are roleplaying this NPC in a game hub.\n"
					"STRICT OUTPUT RULES (must follow):\n"
					"1) Return ONLY the NPC reply text.\n"
					"2) Maximum 2 sentences total. Never exceed 2 sentences.\n"
					"3) No speaker labels, names, prefixes, or transcript text.\n"
					"4) Do not repeat the player message.\n"
					"Name: " + npcInfo.name + "\n" +
					"Background: " + npcInfo.background + "\n" +
					"Player said: " + m_npcInputBuffer + "\n"
                    "Now reply as this NPC in at most 2 immersive sentences.";

				enqueLLMJob(LLMJobType::NPC_REPLY, m_activeNPCId, -1, prompt);
				m_npcInputBuffer.clear();
			}
		}

		return;
	}

	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressed->scancode)
		{
		case sf::Keyboard::Scancode::Escape:
            m_menuUI.setScreen(MenuScreen::PAUSE_MENU_SCREEN);
			break;

		case sf::Keyboard::Scancode::Num0:
			if (m_gameMode != GameMode::DUNGEON || m_activeRoomId < 0 || m_activeRoomId >= static_cast<int>(m_roomPlans.size()))
			{
				std::cout << "No active dungeon room for LLM prompt.\n";
				break;
			}
			LLM_GenerateRoomInfo();
			break;

		case sf::Keyboard::Scancode::R:
			if (m_gameMode == GameMode::HUB)
				startDungeonRun();
			else
				gameStart();
			break;

		case sf::Keyboard::Scancode::V:
			m_isPlayerCamera = !m_isPlayerCamera;
			break;

		case sf::Keyboard::Scancode::Q:
		{
			const int weaponCount = static_cast<int>(m_player.getWeaponLoadout().size());
			if (weaponCount > 0)
			{
				int nextWeaponId = m_player.getCurrentWeaponID() - 1;
				if (nextWeaponId < 0)
					nextWeaponId = weaponCount - 1;
				m_player.setCurrentWeaponID(nextWeaponId);
			}
			break;
		}

		case sf::Keyboard::Scancode::E:
		{
			const int weaponCount = static_cast<int>(m_player.getWeaponLoadout().size());
			if (weaponCount > 0)
			{
				int nextWeaponId = m_player.getCurrentWeaponID() + 1;
				if (nextWeaponId >= weaponCount)
					nextWeaponId = 0;
				m_player.setCurrentWeaponID(nextWeaponId);
			}
			break;
		}

		default:
			break;
		};
	}
}


////////////////////////////////////////////////////////////
/**
 * @brief Advances simulation by one frame.
 *
 * Updates input state, game mode logic, player/enemies/projectiles, collisions,
 * hub interactions, and LLM job polling.
 *
 * @param dt Delta time in seconds.
 */
void Game::update(float dt)
{
 if (m_menuUI.isGameplayScreen())
		m_currentRunStats.timeSeconds += dt;

	const Vector2i mousePos = Mouse::getPosition(m_window);
	sf::View inputView;
	if (m_isPlayerCamera) {
		inputView = m_playerCamera;
		inputView.setCenter(m_player.getPosition());
	}
	else {
		inputView = m_floorCamera;
	}
	const sf::Vector2f mousePosF = static_cast<sf::Vector2f>(m_window.mapPixelToCoords(mousePos, inputView));

	InputManager::update();

	// Game manager controls
	ControllerInputHandler();

    if (!m_menuUI.isGameplayScreen())
	{
		return;
	}	

	if( m_gameMode == GameMode::DUNGEON && m_player.isDead())
	{
		std::cout << "Player has died. Showing game over screen...\n";
       m_currentRunStats.coinsCollected = std::max(0, m_coins - m_runStartCoins);
		const float denom = std::max(1.f, m_currentRunStats.timeSeconds);
		m_currentRunStats.highscore = (m_currentRunStats.kills * (m_currentRunStats.coinsCollected + m_currentRunStats.healthPacksCollected + m_currentRunStats.ammoPacksCollected)) / denom;
		m_lastRunStats = m_currentRunStats;
		m_hasLastRunStats = true;
		m_menuUI.setGameOverStats(m_lastRunStats.kills, m_lastRunStats.timeSeconds, m_lastRunStats.coinsCollected, m_lastRunStats.healthPacksCollected, m_lastRunStats.ammoPacksCollected, m_lastRunStats.highscore);
		m_questManager.finaliseRun();
        m_menuUI.setScreen(MenuScreen::GAME_OVER_SCREEN);
		return;
	}
	updateActiveRoom();

    // Update player
	if (!(m_gameMode == GameMode::HUB && m_isNPCDialogueOpen))
		m_player.update(dt, mousePosF, mousePos, m_gameProjectiles, m_activeDamageTriggers);

	// Enemy Management
	for (auto enemy_it = m_enemies.begin(); enemy_it != m_enemies.end();)
	{
		auto& enemy = *enemy_it;
		enemy->setTarget(m_player.getPosition());
		if (auto* turret = dynamic_cast<TurretEnemy*>(enemy.get()))
			turret->update(dt, m_gameProjectiles);
		else if (auto* grunt = dynamic_cast<GruntEnemy*>(enemy.get()))
			grunt->update(dt, m_activeDamageTriggers);
		else
			enemy->update(dt);

		// Check if enemy is dead and remove if so
		if (enemy->isDead())
		{
			m_questManager.recordEnemyKill(enemy->getEnemyType());
         m_currentRunStats.kills++;
			trySpawnPickup(enemy->getPosition(), 64.f, 40);
			enemy_it = m_enemies.erase(enemy_it);
		}
		else
			enemy_it++;
	}

	// Projectile Management
	for (auto it = m_gameProjectiles.begin(); it != m_gameProjectiles.end();)
	{
		(*it)->update(dt);
		if ((*it)->shouldDestroy())
			it = m_gameProjectiles.erase(it);
		else
			++it;
	}

	// Instantiable trigger management
	for (auto it = m_activeDamageTriggers.begin(); it != m_activeDamageTriggers.end();)
	{
		(*it)->update(dt);
		if ((*it)->shouldDestroy())
			it = m_activeDamageTriggers.erase(it);
		else
			++it;
	}

	//Collision checks
	CollisionChecks();

	// Spawnable management
	for (auto it = m_gamePickups.begin(); it != m_gamePickups.end();)
	{
		if ((*it)->shouldDestroy())
		it = m_gamePickups.erase(it);
		else
			++it;
	}

	// Wave management for combat rooms
	if (m_isInRoom)
	{
		if (m_roomPlans[m_activeRoomId].type == RoomType::COMBAT && m_isInCombat)
			ManageWave();
	}

	// Dungeon management for advancing floors
	if (m_requestNextFloor)
	{
		m_requestNextFloor = false;
		m_dungeonPlan.advanceFloor();

		if (m_dungeonPlan.isDungeonComplete)
		{
			enterHubWorld();
		}
		else
			loadNewFloor();
	}

	// LLM polling
	if (auto initOk = m_llmManager.tryConsumeInitResult())
	{
		if (*initOk) {
			std::cout << "LLM initialized successfully.\n";
		}
		else {
			std::cout << "LLM failed to initialize.\n";
			m_llmManager.initAsync("ASSETS/LLM/MODELS/Llama-3.2-1B-Instruct-Q4_K_M.gguf"); // Retry initialization
		}
	}

	processLLMQueue();

	if (auto response = m_llmManager.tryConsumeLatestResponse())
	{
		handleLLMResponse(*response);
	}

	if (m_roomDescriptionTtl > 0)
		m_roomDescriptionTtl -= dt;

	// Hub management
	if (m_gameMode == GameMode::HUB)
	{
		updateHubShops();
	}

	m_hud.update(m_player.getHealth(), m_player.getMaxHealth(), m_player.getAmmo(), m_player.getMaxAmmo(), m_coins, m_questManager.getActiveQuest());
}

////////////////////////////////////////////////////////////
/**
 * @brief Renders the current frame.
 *
 * Draws room instances, entities, hub overlays, and UI screens.
 */
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));

	if (m_isPlayerCamera)
	{
		m_playerCamera.setCenter(m_player.getPosition());
		m_gameCamera = m_playerCamera;
	}
	else
		m_gameCamera = m_floorCamera;

	m_window.setView(m_gameCamera);
	
	for (auto& roomInstance : m_roomInstances) {
		roomInstance.render(m_window);
	}

	m_player.render(m_window);

	for (auto& enemy : m_enemies) {
		enemy->render(m_window);
	}

	for (auto& bullet : m_gameProjectiles) {
		bullet->render(m_window);
	}

	for(auto& trigger:m_activeDamageTriggers) {
		trigger->render(m_window);
	}

	for(auto& pickup:m_gamePickups) {
		pickup->render(m_window);
	}

	for(auto& chest:m_gameChests) {
		chest->render(m_window);
	}

	if (m_gameMode == GameMode::HUB)
	{
		m_window.draw(m_weaponShop);
		m_window.draw(m_cosmeticShop);
		m_window.draw(m_armoryShop);
		m_window.draw(m_playerShop);
		m_window.draw(m_jobBoard);
		renderHubShopPrompt();

		for (auto& npc : m_hubNPCs) {
			npc.render(m_window);
		}
	}

	m_window.setView(m_window.getDefaultView());

	// UI rendering
    if (m_menuUI.isGameplayScreen())
		m_hud.render(m_window);
	else
       m_menuUI.render(m_window);

	// Room description rendering
	if (m_menuUI.isGameplayScreen() && m_roomDescriptionTtl > 0.f && !m_latestRoomDescription.empty())
	{
		sf::RectangleShape panel;
		panel.setSize(sf::Vector2f(static_cast<float>(m_window.getSize().x) - 40.f, 80.f));
		panel.setOrigin(panel.getSize() / 2.f);
		panel.setPosition(sf::Vector2f(m_window.getSize().x / 2.f, m_window.getSize().y - 60.f));
		panel.setFillColor(sf::Color(0, 0, 0, 170));
		panel.setOutlineThickness(1.f);
		panel.setOutlineColor(sf::Color::White);
		m_window.draw(panel);

		m_roomDescriptionText.setString("Room Insight: " + m_latestRoomDescription);
		m_roomDescriptionText.setOrigin(sf::Vector2f(m_roomDescriptionText.getLocalBounds().size.x / 2.f, m_roomDescriptionText.getLocalBounds().size.y / 2.f));
		m_roomDescriptionText.setPosition({panel.getPosition().x, panel.getPosition().y - 10.f});
		m_window.draw(m_roomDescriptionText);
	}

#ifdef TEST_FPS
	m_window.draw(x_updateFPS); //ups is 60 and dps is 61
	m_window.draw(x_drawFPS);
#endif
	m_window.display();
}

///////////////////////////////////////////////////////////
#pragma region GAME MANAGEMENT
/**
 * @brief Starts a new game from the menu and resets persistent progression.
 */
void Game::startNewGame()
{
	m_coins = 1000;
	m_questManager.resetBoard();

	m_pistolUpgradeLevel = 1;
	m_arUpgradeLevel = 1;
	m_shotgunUpgradeLevel = 1;
	m_armoryCatalogIndex = 0;

	m_playerHealthUpgradeLevel = 1;
	m_playerSpeedUpgradeLevel = 1;
	m_playerAmmoUpgradeLevel = 1;
	m_playerColorIndex = 0;
	resetGame();
	enterHubWorld();
}

/**
 * @brief Resets runtime state for a fresh hub session or dungeon run.
 *
 * Clears spawned entities, resets cameras and flags, and reinitializes generators.
 */
void Game::resetGame()
{
	m_playerCamera = m_window.getDefaultView();
	m_floorCamera = m_window.getDefaultView();
	m_floorCamera.zoom(12.f);
	m_gameCamera = m_playerCamera;
	m_isPlayerCamera = true;

	m_player.init();
	m_player.applyUpgrade(m_playerHealthUpgradeLevel, m_playerSpeedUpgradeLevel, m_playerAmmoUpgradeLevel);
	m_isInCombat = false;
	m_isInRoom = true;
	m_activeShop = HubShopType::NONE_SHOP;
	m_isAtJobBoard = false;
	m_blockJobBoardInteract = false;
	
	m_hubNPCs.clear();
	m_activeNPCId = -1;
	m_isNPCDialogueOpen = false;
	m_selectedResponse = 0;
	m_currentDialogueOptions.clear();

	m_enemies.clear();

	m_roomPlans.clear();
	m_roomInstances.clear();
	m_roomWorldPositions.clear();
	m_activeRoomId = -1;

	m_floorPlan = FloorPlan();
	m_floorGenerator = FloorGenerator();
	m_floorLayout = FloorLayout();
	m_floorInstanceGenerator = FloorLayoutGenerator();

	m_dungeonPlan = DungeonPlan();
	m_requestNextFloor = false;

	m_waveCounter = 0;

	m_gamePickups.clear();
	m_gameChests.clear();
}

/**
 * @brief Starts a dungeon session by resetting state and generating the first floor.
 */
void Game::gameStart()
{
	resetGame();
	m_gameMode = GameMode::DUNGEON;

	m_dungeonPlan.start(rand() % 50000);

	loadNewFloor();
}

/**
 * @brief Convenience wrapper to start a new dungeon run.
 */
void Game::startDungeonRun()
{
	gameStart();
}

/**
 * @brief Manages wave-based combat for combat rooms.
 *
 * When a wave is cleared, either spawns the next wave or marks the room cleared.
 */
void Game::ManageWave()
{
	// check if all enemies are dead
	if (m_enemies.empty()) {
		m_waveCounter--;
		if (m_waveCounter > 0) {	// start new wave
			std::cout << "New Wave Starting! Remaining Waves: " << m_waveCounter << endl;
			m_roomPlans[m_activeRoomId] = CombatRoom().generateNewWave(m_roomPlans[m_activeRoomId]);
			spawnEnemies(m_activeRoomId);
			generateRoom(m_activeRoomId);
		}
		else {	// end combat
			std::cout << "Combat Room Cleared at ID " << m_activeRoomId << "!\n";
			m_roomPlans[m_activeRoomId] = CombatRoom().setRoomCleared(m_roomPlans[m_activeRoomId]);
			generateRoom(m_activeRoomId);
			m_isInCombat = false;
		}
	}
}

#pragma endregion


////////////////////////////////////////////////////////////
#pragma region UPDATE SUBFUNCTIONS
/**
 * @brief Performs collision detection and collision-triggered gameplay interactions.
 */
void Game::CollisionChecks()
{	
	// ------------------- ENEMY CHECKS --------------------
	for (auto& enemy : m_enemies)
	{
		// Enemy -> Player
		if (CollisionCheck::areColliding(m_player, *enemy)) {
			std::cout << "Player collided with enemy!\n";
			
			break;
		}
	}

	// ------------------- BULLET CHECKS -------------------
	for (auto& bullet : m_gameProjectiles)
	{
		if (bullet->shouldDestroy())
			continue;

		// Bullet -> Enemy (Player Projectiles)
		for (auto& enemy : m_enemies) {
			if (enemy->isDead())
				continue;

			if (CollisionCheck::areColliding(*bullet, *enemy))
			{
				enemy->takeDamage(bullet->applyDamage());
				bullet->destroy();
				break;
			}
		}

		// Bullet -> Player (Enemy Projectiles)
		if (CollisionCheck::areColliding(m_player, *bullet))
		{
			m_player.takeDamage(bullet->applyDamage());
			bullet->destroy();
			break;
		}
	}

	// ------------------- DAMAGE TRIGGER CHECKS -------------------
	for (auto& trigger : m_activeDamageTriggers)
	{
		if (trigger->shouldDestroy())
			continue;

		// Trigger -> Player
		if (CollisionCheck::areColliding(*trigger, m_player))
		{
			m_player.takeDamage(trigger->damage());
			break;
		}

		// Trigger -> Enemy
		for (auto& enemy : m_enemies) {
			if (enemy->isDead())
				continue;

			if (CollisionCheck::areColliding(*trigger, *enemy))
			{
				enemy->takeDamage(trigger->damage());
				break;
			}
		}
	}

	// -------------------- PICKUP TRIGGER CHECKS -------------------
	for (auto& pickup : m_gamePickups)
	{
		if (pickup->shouldDestroy())
			continue;

		// Pickup -> Player
		if (CollisionCheck::areColliding(*pickup, m_player))
		{
			switch (pickup->getType())
			{
			case PickupType::HEALTH: {
				m_questManager.recordPickup(PickupType::HEALTH);
               m_currentRunStats.healthPacksCollected++;
				m_player.heal(pickup->getEffectAmount());
				std::cout << "Player picked up health! Current health: " << m_player.getHealth() << endl;
				pickup->destroy();
				break;
			}
			case PickupType::AMMO: {
				m_questManager.recordPickup(PickupType::AMMO);
                m_currentRunStats.ammoPacksCollected++;
				m_player.addAmmo(pickup->getEffectAmount());
				std::cout << "Player picked up ammo! Current ammo: " << m_player.getAmmo() << endl;
				pickup->destroy();
				break;
			}
			case PickupType::SINGLE_COIN: {
				m_coins += pickup->getEffectAmount();
				std::cout << "Player picked up a coin! Current coins: " << m_coins << endl;
				pickup->destroy();
				break;
			}
			case PickupType::CHEST_COIN: {
				m_coins += pickup->getEffectAmount();
				std::cout << "Player picked up a bag of coins! Current coins: " << m_coins << endl;
				pickup->destroy();
				break;
			}
			}
		}
	}

	// ------------------- CHEST TRIGGER CHECKS -------------------
	for (auto& chest : m_gameChests)
	{
		if (chest->isOpened())
			continue;

		// Chest -> Player
		if (CollisionCheck::areColliding(*chest, m_player))
		{
			chest->open();

			const sf::FloatRect bounds = chest->getCollisionBounds();
			const sf::Vector2f chestCenter = sf::Vector2f(bounds.position.x + bounds.size.x / 2, bounds.position.y + bounds.size.y / 2);
			spawnRandomPickup(chestCenter, 64, true);
			break;
		}
	}

	// ------------------- NPC INTERACTION CHECKS -------------------
	for (int i = 0; i < m_hubNPCs.size(); i++) {
		
		// NPC -> Player
		m_activeNPCId = -1; // reset active NPC ID before checks
		if (CollisionCheck::areColliding(m_player, m_hubNPCs[i]))
		{
			m_activeNPCId = i;
			break;
		}
	}

	// -------------------- ENVIRONMENT CHECKS -------------------
	for (auto& roomInstance : m_roomInstances)
	{
		for (auto& collisionObject : roomInstance.getStaticRoomColliders())
		{
			// ____________________________ SHAPE COLLISION CHECKS ____________________________ //
			// Wall or Door collision checks
			if (collisionObject.getCollisionProfile().layer == CollisionLayer::WALL_LAYER ||
				collisionObject.getCollisionProfile().layer == CollisionLayer::DOOR_LAYER)
			{
				// Wall/Door -> Player
				if (CollisionCheck::areColliding(m_player, collisionObject))
				{
					m_player.hitWall();
					break;
				}

				// Wall/Door -> Projectiles
				for (auto& bullet : m_gameProjectiles)
				{
					if (bullet->shouldDestroy())
						continue;

					if (CollisionCheck::areColliding(*bullet, collisionObject)) {
						bullet->destroy();
						break;
					}
				}

				// Wall/Door -> Enemy
				for (auto& enemy : m_enemies) {
					if (enemy->isDead())
						continue;

					if (CollisionCheck::areColliding(*enemy, collisionObject)) {
						enemy->hitWall();
						break;
					}
				}
			}

			// ____________________________ TRIGGER COLLISION CHECKS ____________________________ //
			// Portal trigger checks
			if (collisionObject.getCollisionProfile().layer == CollisionLayer::PORTAL_TRIGGER_LAYER)
			{
				// Portal Trigger -> Player
				if (CollisionCheck::areColliding(collisionObject, m_player))
				{
					if (InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space))
					{
						if (m_gameMode == GameMode::HUB)
							startDungeonRun();
						else
							m_requestNextFloor = true;
					}
					break;
				}
			}

			// Door trigger checks
			if (collisionObject.getCollisionProfile().layer == CollisionLayer::DOOR_TRIGGER_LAYER)
			{
				// Door Trigger -> Player
				if (CollisionCheck::areColliding(collisionObject, m_player))
				{
					// Combat Door Trigger
					if (m_roomPlans[m_activeRoomId].type == RoomType::COMBAT) {
						if (!m_roomPlans[m_activeRoomId].isCleared && !m_isInCombat)
						{
							m_roomPlans[m_activeRoomId] = CombatRoom().generateNewWave(m_roomPlans[m_activeRoomId]);
							spawnEnemies(m_activeRoomId);
							m_isInCombat = true;
							m_waveCounter = rand() % 3 + 1;
							std::cout << "Starting combat room wave!\nWaves Remaining: " << m_waveCounter << "\n";
							generateRoom(m_activeRoomId);
						}
					}
					break;
				}
			}

		}
	}
}

/**
 * @brief Updates `m_activeRoomId` based on the player's position.
 */
void Game::updateActiveRoom()
{
	for (RoomPlan room : m_roomPlans)
	{
		Vector2f roomWorldPos = m_roomWorldPositions[room.id];
		float ts = room.tileSize;

		FloatRect roomBounds(roomWorldPos - Vector2f(ts / 2, ts / 2), Vector2f(room.width * ts, room.height * ts));
		if (roomBounds.contains(m_player.getPosition()))
		{
			m_activeRoomId = room.id;
			m_isInRoom = true;
			break;
		}
		else
		{
			m_isInRoom = false;
			m_activeRoomId = -1;
		}
	}
}

/**
 * @brief Handles controller input affecting game state and UI.
 */
void Game::ControllerInputHandler()
{
	if (InputManager::pad().pressed(GamepadButton::Start))
	{
	#pragma region Menu UI
		if (m_menuUI.isGameplayScreen())
		{
			m_menuUI.setScreen(MenuScreen::PAUSE_MENU_SCREEN);
		}
		else if (m_menuUI.screen() == MenuScreen::PAUSE_MENU_SCREEN)
		{
			m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		}
		else if (m_menuUI.screen() == MenuScreen::QUEST_BOARD_SCREEN)
		{
			m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
			m_blockJobBoardInteract = false; // Allow quest board to be opened again when leaving the quest board screen
		}
		return;

	}

	if (!m_menuUI.isGameplayScreen())
	{
		m_menuUI.processControllerNavigation(
			InputManager::pad().pressed(GamepadButton::DPadUp),
			InputManager::pad().pressed(GamepadButton::DPadDown),
			InputManager::pad().pressed(GamepadButton::A),
			InputManager::pad().pressed(GamepadButton::Start));
		applyMenuAction(m_menuUI.consumeAction());
		return;
	#pragma endregion
	}

	if (InputManager::pad().pressed(GamepadButton::Select))
	{
		m_isPlayerCamera = !m_isPlayerCamera;
	}
	if (InputManager::pad().pressed(GamepadButton::DPadUp))
	{
		if (m_gameMode != GameMode::DUNGEON || m_activeRoomId < 0 || m_activeRoomId >= static_cast<int>(m_roomPlans.size()))
		{
			std::cout << "No active dungeon room for LLM prompt.\n";
			return;
		}
		LLM_GenerateRoomInfo();
	}
}

#pragma endregion


///////////////////////////////////////////////////////////
#pragma region ROOM MANAGEMENT
/**
 * @brief Builds a `RoomInstance` from its matching `RoomPlan`.
 * @param roomId Identifier of the room to build.
 */
void Game::generateRoom(int roomId)
{
	RoomPlan& roomPlan = m_roomPlans[roomId];
	Vector2f roomWorldPos = m_roomWorldPositions[roomId];
	m_roomInstances[roomId].buildFromPlan(roomPlan, roomWorldPos);
}

/**
 * @brief Spawns the player using the room's player spawner.
 * @param roomId Identifier of the room to spawn the player in.
 */
void Game::spawnPlayer(const int roomId)
{
	RoomPlan& roomPlan = m_roomPlans[roomId];
	Vector2f roomWorldPos = m_roomWorldPositions[roomId];

	// Spawn room Spawner
	if (roomPlan.type == RoomType::SPAWN)
	{
		for (auto& spawnPoint : roomPlan.spawners)
		{
			if (spawnPoint.type == SpawnerType::PlayerSpawner) {
				Vector2f spawnPos = roomWorldPos + static_cast<Vector2f>(spawnPoint.tilePos) * roomPlan.tileSize;
				m_player.setSpawnPosition(spawnPos);
				m_activeRoomId = roomId;
				m_isInRoom = true;
			}
		}
	}
}

/**
 * @brief Spawns enemies in a combat room using enemy spawners.
 * @param roomId Identifier of the room to spawn enemies in.
 */
void Game::spawnEnemies(const int roomId)
{
	// Combat room Spawner
	m_enemies.clear();
	RoomPlan& roomPlan = m_roomPlans[roomId];
	Vector2f roomWorldPos = m_roomWorldPositions[roomId];

	if (roomPlan.type == RoomType::COMBAT)
	{
		int totalEnemyTypes = EnemyType::ENEMY_COUNT;
		for (auto& spawnPoint : roomPlan.spawners)
		{
			if (spawnPoint.type == SpawnerType::EnemySpawner) {
				Vector2f spawnPos = roomWorldPos + static_cast<Vector2f>(spawnPoint.tilePos) * roomPlan.tileSize;

				int enemyToSpawn = rand() % totalEnemyTypes;
				if (enemyToSpawn == 0)
					m_enemies.push_back(std::make_unique<GruntEnemy>(spawnPos, 50));
				else if (enemyToSpawn == 1)
					m_enemies.push_back(std::make_unique<TurretEnemy>(spawnPos, 75));
			}
		}
	}
}

/**
 * @brief Spawns a random pickup at a world location.
 * @param worldPos World position to spawn at.
 * @param tilesize Tile size used for pickup sizing.
 * @param isFromChest Whether the spawn originates from a chest (affects coin type).
 */
void Game::spawnRandomPickup(const sf::Vector2f& worldPos, float tilesize, bool isFromChest)
{
	PickupType type = PickupType::PICKUP_COUNT;
	const int roll = rand() % 3;
	if (roll == 0)
		type = PickupType::HEALTH;
	else if (roll == 1)
		type = PickupType::AMMO;
	else {
		if (isFromChest)
			type = PickupType::CHEST_COIN;
		else
			type = PickupType::SINGLE_COIN;
	}

	m_gamePickups.push_back(std::make_unique<Pickup>(type, worldPos, tilesize));
}

/**
 * @brief Rolls a chance to spawn a pickup at a world location.
 * @param worldPos World position to spawn at.
 * @param tilesize Tile size used for pickup sizing.
 * @param spawnChance Chance percentage (0-100).
 */
void Game::trySpawnPickup(const sf::Vector2f& worldPos, float tilesize, int spawnChance)
{
	const int chance = rand() % 100;
	if (chance <= spawnChance)
		spawnRandomPickup(worldPos, tilesize, false);
}


#pragma endregion


///////////////////////////////////////////////
#pragma	region FLOOR MANAGEMENT
/**
 * @brief Builds `RoomInstance`s for all rooms in the current floor and positions them based on the floor layout.
 *
 * Calculates cell sizes for grid-based layout, determines world positions for each room, generates room instances,
 * spawns the player in the spawn room, and builds corridors between rooms.

*/
void Game::buildFloorInstance()
{
	const int roomCount = m_floorPlan.rooms.size();
	if (roomCount <= 0) return;

	// Calculate the world size of each cell in the grid layout based on the largest room dimensions and a fixed gap between rooms
	const float tileSize = m_roomPlans[0].tileSize;
	const int gapTiles = 5;

	int maxWidth = 0;
	int maxHeight = 0;
	for (auto& roomPlan : m_roomPlans)
	{
		maxWidth = std::max(maxWidth, roomPlan.width);
		maxHeight = std::max(maxHeight, roomPlan.height);
	}

	const float cellWorldW = (maxWidth + gapTiles) * tileSize;
	const float cellWorldH = (maxHeight + gapTiles) * tileSize;

	m_roomInstances.clear();
	m_roomWorldPositions.clear();

	m_roomInstances.resize(roomCount);
	m_roomWorldPositions.resize(roomCount);

	// Calculate world positions for each room based on their grid positions and the calculated cell world size, then build room instances from their plans
	for (int roomId = 0; roomId < roomCount; roomId++)
	{
		Vector2i gridPos = m_floorLayout.roomGridPositions[roomId];
		Vector2f cellCenter = Vector2f(gridPos.x * cellWorldW, gridPos.y * cellWorldH);

		const RoomPlan& roomPlan = m_roomPlans[roomId];
		Vector2f halfRoomSize = Vector2f((roomPlan.width - 1) * tileSize, (roomPlan.height - 1) * tileSize) * .5f;

		Vector2f worldPos = cellCenter - halfRoomSize;

		m_roomWorldPositions[roomId] = worldPos;
		generateRoom(roomId);

		// Spawn player in spawn room
		if (m_roomPlans[roomId].type == RoomType::SPAWN)
			spawnPlayer(roomId);
	}

	buildCorridors();
}

/**
 * @brief Builds corridor instances connecting rooms described by the floor plan.
 */
void Game::buildCorridors()
{
	if (m_roomPlans.empty()) return;

	const float tileSize = m_roomPlans[0].tileSize;

#pragma region Corridor Helper Lambdas
	auto getDoor = [&](const RoomPlan& roomPlan, DoorDirection dir)
		{
			for (auto& door : roomPlan.doors)
				if (door.direction == dir)
					return &door;
		};

	// Returns the opposite direction of the given door direction (e.g. NORTH -> SOUTH, EAST -> WEST)
	auto opposite = [&](DoorDirection dir)
		{
			return RoomDoorUtils::opposite(dir);
		};

	// Determines the direction of the door between two rooms based on their grid positions in the floor layout
	auto directionBetweenRooms = [&](int fromId, int toId)
		{
			// Determine the direction of the door based on the grid positions of the two rooms in the floor layout
			Vector2i a = m_floorLayout.roomGridPositions[fromId];
			Vector2i b = m_floorLayout.roomGridPositions[toId];
			Vector2i diff = b - a;
			// The difference in grid positions should indicate the direction of the door
			if (diff.x != 0 && (std::abs(diff.x) >= std::abs(diff.y) || diff.y == 0))
			{
				if (diff.x > 0) return DoorDirection::EAST;
				else return DoorDirection::WEST;
			}
			else
			{
				if (diff.y > 0) return DoorDirection::SOUTH;
				else return DoorDirection::NORTH;
			}
		};

#pragma endregion

	int corridorIdCounter = 0;

	
	for (const auto& edge : m_floorPlan.edges)
	{
		// 1) Declare variables for the two rooms connected by this edge and the directions of the doors between them
		const int aId = edge.id_a;
		const int bId = edge.id_b;

		const DoorDirection dirFromAtoB = directionBetweenRooms(aId, bId);
		const DoorDirection dirFromBtoA = opposite(dirFromAtoB);

		const RoomPlan& roomA = m_roomPlans[aId];
		const RoomPlan& roomB = m_roomPlans[bId];
		const DoorPlan* doorA = getDoor(roomA, dirFromAtoB);
		const DoorPlan* doorB = getDoor(roomB, dirFromBtoA);
		if (!doorA || !doorB) continue;	// if either room doesn't have the expected door, skip this edge (invalid edge)

		const bool isHorizontal = (dirFromAtoB == DoorDirection::EAST || dirFromAtoB == DoorDirection::WEST);

		const int walkableThickness = std::min(doorA->spanTiles, doorB->spanTiles);
		const int corridorThickness = walkableThickness + 2;

		Vector2f start;
		Vector2f end;

		Vector2f corridorWorldPos;

		RoomPlan corridor;
		corridor.id = corridorIdCounter++;
		corridor.type = RoomType::CORRIDOR;
		corridor.seed = 0;
		corridor.tileSize = tileSize;

		// 3) Calculate the world positions of the two doors and use them to determine the start and end points of the corridor
		if (isHorizontal)
		{
			// For horizontal corridors, the Y position of the corridor will be centered on the doors, and the X positions will be at the walls of the rooms (door position + half tile size)
			const float doorA_WallX = m_roomWorldPositions[aId].x + (doorA->tileStartPos.x * tileSize);
			const float doorB_WallX = m_roomWorldPositions[bId].x + (doorB->tileStartPos.x * tileSize);

			// To center the corridor on the doors, we need to calculate the center Y position of the corridor based on the door positions and their span
			const float doorA_CenterY = m_roomWorldPositions[aId].y + ((doorA->tileStartPos.y + (doorA->spanTiles - 1) * .5f) * tileSize);
			const float doorB_CenterY = m_roomWorldPositions[bId].y + ((doorB->tileStartPos.y + (doorB->spanTiles - 1) * .5f) * tileSize);
			// The center Y position of the corridor should be the average of the two door center Y positions to ensure it is centered between them
			const float centerY = (doorA_CenterY + doorB_CenterY) * .5f;
			// The start and end points of the corridor will be at the wall X positions of the rooms and the center Y position calculated above
			start = Vector2f(doorA_WallX, centerY);
			end = Vector2f(doorB_WallX, centerY);

			if (end.x < start.x) std::swap(start, end);	// ensure start is always left of end for horizontal corridors
			const int lengthTiles = static_cast<int>((end.x - start.x) / tileSize) + 1;

			corridor.width = lengthTiles;
			corridor.height = corridorThickness;

			// 2) Create a corridor plan with the appropriate dimensions and layout based on the positions of the two doors and the direction of the corridor (horizontal or vertical)
			corridor.tileMap.assign(corridor.width * corridor.height, Tile::WALL);

			for (int row = 1; row <= walkableThickness; row++)
				for (int col = 0; col < corridor.width; col++)
					corridor.setTile(row, col, Tile::FLOOR);

			// Place corridor that (0,0) center is at (start.x, centerY - halfHeightSpan) so that the corridor is centered on the doors
			const float halfHeightSpan = ((corridor.height - 1) * 0.5f) * tileSize;
			corridorWorldPos = Vector2f(start.x, centerY - halfHeightSpan);
		}
		else
		{
			// For vertical corridors, the X position of the corridor will be centered on the doors, and the Y positions will be at the walls of the rooms (door position + half tile size)
			const float doorA_WallY = m_roomWorldPositions[aId].y + (doorA->tileStartPos.y * tileSize);
			const float doorB_WallY = m_roomWorldPositions[bId].y + (doorB->tileStartPos.y * tileSize);
			// To center the corridor on the doors, we need to calculate the center X position of the corridor based on the door positions and their span
			const float doorA_CenterX = m_roomWorldPositions[aId].x + ((doorA->tileStartPos.x + (doorA->spanTiles - 1) * .5f) * tileSize);
			const float doorB_CenterX = m_roomWorldPositions[bId].x + ((doorB->tileStartPos.x + (doorB->spanTiles - 1) * .5f) * tileSize);
			// The center X position of the corridor should be the average of the two door center X positions to ensure it is centered between them
			const float centerX = (doorA_CenterX + doorB_CenterX) * .5f;
			// The start and end points of the corridor will be at the wall Y positions of the rooms and the center X position calculated above
			start = Vector2f(centerX, doorA_WallY);
			end = Vector2f(centerX, doorB_WallY);

			if (end.y < start.y) std::swap(start, end);	// ensure start is always above end for vertical corridors
			const int lengthTiles = static_cast<int>((end.y - start.y) / tileSize) + 1;

			corridor.width = corridorThickness;
			corridor.height = lengthTiles;

			// 2) Create a corridor plan with the appropriate dimensions and layout based on the positions of the two doors and the direction of the corridor (horizontal or vertical)
			corridor.tileMap.assign(corridor.width* corridor.height, Tile::WALL);

			for (int col = 1; col <= walkableThickness; col++)
				for (int row = 0; row < corridor.height; row++)
					corridor.setTile(row, col, Tile::FLOOR);

			// Place corridor that (0,0) center is at (centerX - halfWidthSpan, start.y) so that the corridor is centered on the doors
			const float halfWidthSpan = ((corridor.width - 1) * 0.5f) * tileSize;
			corridorWorldPos = Vector2f(centerX - halfWidthSpan, start.y);
		}

		// 4) Store the corridor plan and its world position, then build a room instance from the corridor plan and add it to the list of room instances
		RoomInstance corridorInst;
		corridorInst.buildFromPlan(corridor, corridorWorldPos);
		m_roomInstances.push_back(corridorInst);
	}

}

/**
 * @brief Generates and loads a new dungeon floor.
 */
void Game::loadNewFloor()
{
	m_enemies.clear();
	m_isInCombat = false;
	m_isInRoom = true;
	m_activeRoomId = -1;
	m_waveCounter = 0;

	int dungeonSeed = m_dungeonPlan.seed;
	int floorId = m_dungeonPlan.currentFloorId;
	m_floorPlan = m_floorGenerator.generateFloorPlan(floorId, dungeonSeed, m_dungeonPlan.isFinalFloor());

	m_roomPlans.clear();
	m_roomPlans.resize(m_floorPlan.rooms.size());

	for (const auto& room : m_floorPlan.rooms)
	{
		switch (room.roomType) {
		case RoomType::SPAWN:
			m_roomPlans[room.id] = SpawnRoom().generateRoomPlan(room.id, room.roomType, dungeonSeed + floorId);
			break;
		case RoomType::PORTAL:
			m_roomPlans[room.id] = PortalRoom().generateRoomPlan(room.id, room.roomType, dungeonSeed + floorId);
			break;
		case RoomType::COMBAT:
			m_roomPlans[room.id] = CombatRoom().generateRoomPlan(room.id, room.roomType, dungeonSeed + floorId);
			break;
		}
	}

	m_floorLayout = m_floorInstanceGenerator.generateFloorLayout(m_floorPlan);

	for (auto& roomPlan : m_roomPlans)
	{
		RoomDoorUtils::clearAllDoors(roomPlan);
	}

	auto directionBetweenRooms = [&](int fromId, int toId)
		{
			// Determine the direction of the door based on the grid positions of the two rooms in the floor layout
			Vector2i a = m_floorLayout.roomGridPositions[fromId];
			Vector2i b = m_floorLayout.roomGridPositions[toId];
			Vector2i diff = b - a;

			// The difference in grid positions should indicate the direction of the door
			if (diff.x != 0 && (std::abs(diff.x) >= std::abs(diff.y) || diff.y == 0))
			{
				if (diff.x > 0) return DoorDirection::EAST;
				else return DoorDirection::WEST;
			}
			else
			{
				if (diff.y > 0) return DoorDirection::SOUTH;
				else return DoorDirection::NORTH;
			}
		};

	for (auto& edge : m_floorPlan.edges)
	{
		int aId = edge.id_a;
		int bId = edge.id_b;

		DoorDirection dirFromAtoB = directionBetweenRooms(aId, bId);
		DoorDirection dirFromBtoA = RoomDoorUtils::opposite(dirFromAtoB);

		RoomDoorUtils::addDoor(m_roomPlans[aId], dirFromAtoB);
		RoomDoorUtils::addDoor(m_roomPlans[bId], dirFromBtoA);
	}

	buildFloorInstance();

	m_gameChests.clear();
	m_gamePickups.clear();

	for (const auto& roomPlan : m_roomPlans)
	{
		int spawnChance = rand() % 100;
		if (spawnChance <= 60 && roomPlan.type != RoomType::COMBAT)
		{
			const sf::Vector2f roomPos = m_roomWorldPositions[roomPlan.id];
			const float tileSize = roomPlan.tileSize;
			const sf::Vector2f roomCenter = roomPos + sf::Vector2f((roomPlan.width * tileSize) / 2, (roomPlan.height * tileSize) / 2);

			m_gameChests.push_back(std::make_unique<Chest>(roomCenter, tileSize));
		}
	}

	m_floorCamera.setCenter(m_roomWorldPositions[0]);
}


#pragma endregion


/////////////////////////////////////////////////////////////////
#pragma region LLM INTERACTION
/**
 * @brief Queues the first set of quest metadata LLM jobs.
 */
void Game::queueQuestMetadataJobs()
{
	m_pendingQuestTitles.clear();
	m_pendingQuestTitles.resize(m_questManager.getBoardQuests().size());
	m_pendingQuestContexts.clear();
	m_pendingQuestContexts.resize(m_questManager.getBoardQuests().size());

	const auto& boardQuests = m_questManager.getBoardQuests();
  if (!boardQuests.empty())
	{
        const QuestData* firstQuest = m_questManager.getBoardQuest(0);
		const std::string context = firstQuest ? getRandomQuestNpcContext(*firstQuest) : "";
		m_pendingQuestContexts[0] = context;
		const std::string prompt = buildQuestTitlePrompt(0, context);
		if (!prompt.empty())
            enqueLLMJob(LLMJobType::QUEST_TITLE, -1, 0, prompt);
	}
}

/**
 * @brief Builds an LLM prompt requesting a quest title.
 * @param questIndex Board index of the quest.
 * @param npcContext Short NPC context line.
 * @return Prompt string, or empty when the quest index is invalid.
 */
std::string Game::buildQuestTitlePrompt(int questIndex, const std::string& npcContext) const
{
	const QuestData* quest = m_questManager.getBoardQuest(questIndex);
	if (!quest)
		return "";

	std::string questTypeStr = (quest->type == QuestType::KILL) ? "Kill" : "Fetch";
	std::string targetStr;

	if (quest->type == QuestType::KILL)
	{
		targetStr = QuestData::getEnemyName(quest->enemyType) + " enemies";
	}
	else
	{
		targetStr = QuestData::getPickupName(quest->pickupType) + " packs";
	}

	std::string prompt =
		"Generate only the quest title for a bulletin board.\n"
		"\n"
		"FORMAT:\n"
		"TITLE: <2 to 5 words>\n"
		"\n"
		"RULES:\n"
		"- Output exactly 1 line.\n"
		"- Begin with 'TITLE: '.\n"
		"- TITLE must be 2 to 5 words.\n"
		"- Preserve the exact quest type, target type, target count, reward, and NPC context.\n"
		"- No extra text.\n"
		"\n"
		"INPUT:\n"
		"Quest Type: " + questTypeStr + "\n"
		"Target: " + std::to_string(quest->targetCount) + " " + targetStr + "\n"
		"Reward: " + std::to_string(quest->rewardCoins) + " coins\n"
		"NPC Context: " + npcContext + "\n"
		"\n"
		"OUTPUT:\n"
		"TITLE: ";

	return prompt;
}

/**
 * @brief Builds an LLM prompt requesting quest lore.
 * @param questIndex Board index of the quest.
 * @param title Title to use as context.
 * @param npcContext Short NPC context line.
 * @return Prompt string, or empty when the quest index is invalid.
 */
std::string Game::buildQuestLorePrompt(int questIndex, const std::string& title, const std::string& npcContext) const
{
	const QuestData* quest = m_questManager.getBoardQuest(questIndex);
	if (!quest)
		return "";

	std::string questTypeStr = (quest->type == QuestType::KILL) ? "Kill" : "Fetch";
	std::string targetStr;

	if (quest->type == QuestType::KILL)
	{
		targetStr = QuestData::getEnemyName(quest->enemyType) + " enemies";
	}
	else
	{
		targetStr = QuestData::getPickupName(quest->pickupType) + " packs";
	}

	std::string prompt =
		"Generate only the quest lore for a bulletin board using the provided title.\n"
		"\n"
		"FORMAT:\n"
       "LORE: <sentence 1>\n"
		"<sentence 2>\n"
		"\n"
		"RULES:\n"
        "- Output exactly 2 lines.\n"
		"- Begin with 'LORE: '.\n"
		"- Use the provided title naturally as context.\n"
		"- LORE must be exactly 2 short sentences.\n"
		"- Preserve the exact quest type, target type, target count, reward, and NPC context.\n"
		"- No extra text.\n"
		"\n"
		"INPUT:\n"
		"Title: " + title + "\n"
		"Quest Type: " + questTypeStr + "\n"
		"Target: " + std::to_string(quest->targetCount) + " " + targetStr + "\n"
		"Reward: " + std::to_string(quest->rewardCoins) + " coins\n"
		"NPC Context: " + npcContext + "\n"
		"\n"
		"OUTPUT:\n"
		"LORE: ";

	return prompt;
}

/**
 * @brief Enqueues an LLM job that generates a short room description.
 */
void Game::LLM_GenerateRoomInfo()
{
	if (!m_llmManager.isReady()) {
		std::cout << "LLM model is not ready.\n";
		return;
	}

	RoomPlan currRoom = m_roomPlans[m_activeRoomId];

	std::string roomTypeStr;
	switch (currRoom.type) {
	case RoomType::CORRIDOR: roomTypeStr = "Corridor"; break;
	case RoomType::SPAWN: roomTypeStr = "Spawn Room"; break;
	case RoomType::PORTAL: roomTypeStr = "Portal Room"; break;
	case RoomType::COMBAT: roomTypeStr = "Combat Room"; break;
	}

	std::string sizeStr;
	int area = currRoom.width * currRoom.height;

	if (area <= 36) sizeStr = "small";
	else if (area <= 100) sizeStr = "medium";
	else sizeStr = "large";

	const std::string prompt =
		"Write exactly 1 short sentence for a " + roomTypeStr + ".\n"
		"\n"
		"RULES:\n"
		"- Output exactly 1 sentence.\n"
		"- Mention the Room Type.\n"
		"- Mention the room size word exactly.\n"
		"- Include one unique environmental object or feature.\n"
		"- Max 18 words.\n"
		"- No extra text.\n"
		"\n"
		"INPUT:\n"
		"Room Type: " + roomTypeStr + "\n"
		"Relative Size: " + sizeStr + "\n"
		"\n"
		"OUTPUT:";

	enqueLLMJob(LLMJobType::ROOM_DESCRIPTION, -1, -1, prompt);
	std::cout << "Enqueued LLM job for room description.\n";
}

/**
 * @brief Enqueues an LLM job into the local job queue.
 * @param jobType Job type.
 * @param npcId NPC identifier for NPC reply jobs.
 * @param questIndex Quest board index for quest metadata jobs.
 * @param prompt Prompt text to send to the model.
 */
void Game::enqueLLMJob(LLMJobType jobType, int npcId, int questIndex, const std::string& prompt)
{
	LLMJobContext job;
	job.jobType = jobType;
	job.npcId = npcId;
	job.questIndex = questIndex;
	job.prompt = prompt;

	m_llmJobQueue.push(job);
}

/**
 * @brief Starts the next queued LLM job if the model is ready.
 */
void Game::processLLMQueue()
{
	if (!m_llmManager.isReady() || m_llmManager.isBusy() || m_llmJobQueue.empty())
		return;

	m_currentLLMJob = m_llmJobQueue.front();
	m_llmJobQueue.pop();

	if(m_llmManager.requestGenerate(m_currentLLMJob.prompt))
		m_hasActiveLLMJob = true;
	else {
		std::cout << "Failed to queue LLM job.\n";
		m_hasActiveLLMJob = false;
	}
}

/**
 * @brief Applies the latest LLM response to game/UI state.
 * @param response Raw model output.
 */
void Game::handleLLMResponse(const std::string& response)
{
	if (!m_hasActiveLLMJob) {
		std::cout << "No active LLM job to handle response for.\n";
		return;
	}

	switch (m_currentLLMJob.jobType)
	{
	case LLMJobType::ROOM_DESCRIPTION:
		m_latestRoomDescription = response;
		m_roomDescriptionTtl = 10.f; // display room description for 10 seconds
		std::cout << "LLM Room Description: " << response << "\n";
		break;

	case LLMJobType::NPC_REPLY:
		m_npcLastResponse = response;
		std::cout << "NPC Reply (npcId: " << m_currentLLMJob.npcId << "): " << response << "\n";
		break;

    case LLMJobType::QUEST_TITLE:
	{
       const int questIndex = m_currentLLMJob.questIndex;
		std::string title = extractTaggedLineValue(response, "TITLE:");
		if (title.empty())
			title = stripTaggedPrefix(extractFirstLine(response), "TITLE:");

		if (questIndex >= 0 && questIndex < static_cast<int>(m_pendingQuestTitles.size()))
			m_pendingQuestTitles[questIndex] = title;

		std::string context;
		if (questIndex >= 0 && questIndex < static_cast<int>(m_pendingQuestContexts.size()))
			context = m_pendingQuestContexts[questIndex];

     const std::string lorePrompt = buildQuestLorePrompt(questIndex, title, context);
		if (!lorePrompt.empty())
			enqueLLMJob(LLMJobType::QUEST_LORE, -1, questIndex, lorePrompt);

		std::cout << "Quest Title (questIndex=" << questIndex << "): " << title << "\n";
		break;
	}

	case LLMJobType::QUEST_LORE:
	{
		const int questIndex = m_currentLLMJob.questIndex;
       std::string lore = formatLoreTwoLines(extractLorePayload(response));

		std::string title;
		if (questIndex >= 0 && questIndex < static_cast<int>(m_pendingQuestTitles.size()))
			title = m_pendingQuestTitles[questIndex];

		if (m_questManager.updateBoardQuestText(questIndex, title, lore))
		{
			m_menuUI.setQuestBoardQuests(m_questManager.getBoardQuests());
		}

		const int nextQuestIndex = questIndex + 1;
		if (nextQuestIndex < static_cast<int>(m_questManager.getBoardQuests().size()))
		{
          const QuestData* nextQuest = m_questManager.getBoardQuest(nextQuestIndex);
			const std::string nextContext = nextQuest ? getRandomQuestNpcContext(*nextQuest) : "";
			if (nextQuestIndex >= 0 && nextQuestIndex < static_cast<int>(m_pendingQuestContexts.size()))
				m_pendingQuestContexts[nextQuestIndex] = nextContext;

			const std::string nextTitlePrompt = buildQuestTitlePrompt(nextQuestIndex, nextContext);
			if (!nextTitlePrompt.empty())
				enqueLLMJob(LLMJobType::QUEST_TITLE, -1, nextQuestIndex, nextTitlePrompt);
		}

      std::cout << "Quest Lore (questIndex=" << questIndex << "): " << lore << "\n";
		break;
	}
	}

	m_hasActiveLLMJob = false;
}

#pragma endregion


////////////////////////////////////////////////////////////////
#pragma region HUB WORLD MANAGEMENT
/**
 * @brief Transitions back to the hub world.
 *
 * Finalises and commits run results (quest rewards), queues quest metadata LLM jobs,
 * and builds the hub room setup.
 */
void Game::enterHubWorld()
{
	resetGame();
	m_questManager.finaliseRun();
	m_coins += m_questManager.commitRunResult();
	queueQuestMetadataJobs();
	m_gameMode = GameMode::HUB;
	buildHubWorld();
	m_requestNextFloor = false;
}

/**
 * @brief Creates the hub world room layout, shops, and NPC placements.
 */
void Game::buildHubWorld()
{
	// Set Room PLan for Hub Room
	RoomPlan hubRoom;
	hubRoom.id = 0;
	hubRoom.type = RoomType::SPAWN;
	hubRoom.seed = 0;
	hubRoom.width = 30;
	hubRoom.height = 20;
	hubRoom.tileSize = 64.f;
	hubRoom.tileMap.assign(hubRoom.width * hubRoom.height, Tile::FLOOR);

	// Build outer walls
	for (int col = 0; col < hubRoom.width; col++) {
		hubRoom.setTile(0, col, Tile::WALL);
		hubRoom.setTile(hubRoom.height - 1, col, Tile::WALL);
	}
	for (int row = 0; row < hubRoom.height; row++) {
		hubRoom.setTile(row, 0, Tile::WALL);
		hubRoom.setTile(row, hubRoom.width - 1, Tile::WALL);
	}

	// Spawn point and Portal placement
	const sf::Vector2i playerSpawnTile(hubRoom.width / 2, hubRoom.height / 2);
	const sf::Vector2i portalTile(hubRoom.width / 2, hubRoom.height / 4);

	const sf::Vector2i gunShopTile(hubRoom.width / 4, hubRoom.height / 2);
	const sf::Vector2i armoryShopTile(hubRoom.width / 4, 3 * hubRoom.height / 4);
	const sf::Vector2i cosmeticShopTile(3 * hubRoom.width / 4, hubRoom.height / 2);
	const sf::Vector2i playerShopTile(3 * hubRoom.width / 4, 3 * hubRoom.height / 4);
	const sf::Vector2i jobBoardTile(hubRoom.width / 2, 3 * hubRoom.height / 4);

	hubRoom.spawners.push_back({ SpawnerType::PlayerSpawner, playerSpawnTile });
	hubRoom.spawners.push_back({ SpawnerType::PortalSpawner, portalTile });
	hubRoom.triggers.push_back({ TriggerType::PortalTrigger, portalTile });

	m_roomPlans.clear();
	m_roomInstances.clear();
	m_roomWorldPositions.clear();

	m_roomPlans.push_back(hubRoom);
	m_roomInstances.resize(m_roomPlans.size());
	m_roomWorldPositions.resize(m_roomPlans.size());

	// For the hub world, we can just place the single room at the origin
	m_roomWorldPositions[0] = sf::Vector2f(0.f, 0.f);
	generateRoom(0);
	spawnPlayer(0);

	// Set active room to hub
	m_activeRoomId = 0;
	m_isInRoom = true;
	m_isInCombat = false;
	m_waveCounter = 0;

	m_playerCamera.setCenter(m_player.getPosition());
	m_floorCamera.setCenter(m_player.getPosition());

	const float tileSize = hubRoom.tileSize;
	const sf::Vector2f worldOrigin = m_roomWorldPositions[0];
	const sf::Vector2f gunShopCenter = worldOrigin + sf::Vector2f(gunShopTile.x * tileSize + tileSize / 2, gunShopTile.y * tileSize + tileSize / 2);
	const sf::Vector2f cosmeticShopCenter = worldOrigin + sf::Vector2f(cosmeticShopTile.x * tileSize + tileSize / 2, cosmeticShopTile.y * tileSize + tileSize / 2);
	const sf::Vector2f armoryShopCenter = worldOrigin + sf::Vector2f(armoryShopTile.x * tileSize + tileSize / 2, armoryShopTile.y * tileSize + tileSize / 2);
	const sf::Vector2f playerShopCenter = worldOrigin + sf::Vector2f(playerShopTile.x * tileSize + tileSize / 2, playerShopTile.y * tileSize + tileSize / 2);

	// Initialize shops
	m_weaponShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_weaponShop.setOrigin(m_weaponShop.getSize() / 2.f);
	m_weaponShop.setPosition(gunShopCenter);
	m_weaponShop.setFillColor(sf::Color(0, 255, 255, 180)); // Cyan color

	m_cosmeticShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_cosmeticShop.setOrigin(m_cosmeticShop.getSize() / 2.f);
	m_cosmeticShop.setPosition(cosmeticShopCenter);
	m_cosmeticShop.setFillColor(sf::Color(255, 0, 255, 180)); // Magenta color

	m_armoryShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_armoryShop.setOrigin(m_armoryShop.getSize() / 2.f);
	m_armoryShop.setPosition(armoryShopCenter);
	m_armoryShop.setFillColor(sf::Color(255, 255, 255, 180)); // White color

	m_playerShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_playerShop.setOrigin(m_playerShop.getSize() / 2.f);
	m_playerShop.setPosition(playerShopCenter);
	m_playerShop.setFillColor(sf::Color(255, 165, 0, 180)); // Orange color

	m_jobBoard.setSize(sf::Vector2f(tileSize * 4.f, tileSize * 2.f));
	m_jobBoard.setOrigin(m_jobBoard.getSize() / 2.f);
	m_jobBoard.setPosition(worldOrigin + sf::Vector2f(jobBoardTile.x * tileSize + tileSize / 2.f, jobBoardTile.y * tileSize + tileSize / 2.f));
	m_jobBoard.setFillColor(sf::Color(120, 84, 54, 200)); // Brown color

	// Initialize hub NPCs
	m_hubNPCs.clear();
	const float shopHalfWidth = m_weaponShop.getSize().x / 2.f;
	const float npcHalfWidth = 25.f;
	const float sideOffset = shopHalfWidth + npcHalfWidth + 16.f;

	const sf::Vector2f rightSideTriggerOffset(-70.f, 0.f);
	const sf::Vector2f leftSideTriggerOffset(70.f, 0.f);

	// NPCs
	for (int i = 0; i < 4; i++) {
		HubNPCInfo npcInfo;
		if (i == 0) {
			npcInfo.name = "Gunther";
			npcInfo.shopType = HubShopType::WEAPON_SHOP;
			npcInfo.personality = NPCPersonality::GREEDY;
			npcInfo.background = "Black Market Dealer";

			m_hubNPCs.push_back(NPC(npcInfo,
				gunShopCenter + sf::Vector2f(sideOffset, 0.f),
				sf::Color(220, 60, 60), // Crimson color
				leftSideTriggerOffset));
		}
		else if (i == 1) {
			npcInfo.name = "Cassandra";
			npcInfo.shopType = HubShopType::COSMETIC_SHOP;
			npcInfo.personality = NPCPersonality::PASSIONATE;
			npcInfo.background = "Former adventurer turned Fashion Guru";

			m_hubNPCs.push_back(NPC(npcInfo,
				cosmeticShopCenter + sf::Vector2f(-sideOffset, 0.f),
				sf::Color(255, 105, 180), // Hot pink color
				rightSideTriggerOffset));
		}
		else if (i == 2) {
			npcInfo.name = "Arnold";
			npcInfo.shopType = HubShopType::ARMORY_SHOP;
			npcInfo.personality = NPCPersonality::PROFESSIONAL;
			npcInfo.background = "Master blacksmith; Highly Intelligent";

			m_hubNPCs.push_back(NPC(npcInfo,
				armoryShopCenter + sf::Vector2f(sideOffset, 0.f),
				sf::Color(192, 192, 192), // Silver color
				leftSideTriggerOffset));
		}
		else if (i == 3) {
			npcInfo.name = "Petra the Doctor";
			npcInfo.shopType = HubShopType::PLAYER_SHOP;
			npcInfo.personality = NPCPersonality::FRIENDLY;
			npcInfo.background = "Skilled medic; Shady";

			m_hubNPCs.push_back(NPC(npcInfo,
				playerShopCenter + sf::Vector2f(-sideOffset, 0.f),
				sf::Color(255, 165, 0), // Orange color
				rightSideTriggerOffset));
		}
	}
}

/**
 * @brief Updates hub interaction state for shops, NPC dialogue, and quest board.
 */
void Game::updateHubShops()
{
	m_activeShop = HubShopType::NONE_SHOP;
	m_isAtJobBoard = false;

	const sf::FloatRect playerBounds = m_player.getCollisionBounds();

	// NPC Interactions
	const bool interactPressed = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
	const bool upPressed = InputManager::pad().pressed(GamepadButton::DPadUp) || Keyboard::isKeyPressed(Keyboard::Key::W);
	const bool downPressed = InputManager::pad().pressed(GamepadButton::DPadDown) || Keyboard::isKeyPressed(Keyboard::Key::S);
	const bool cancelPressed = InputManager::pad().pressed(GamepadButton::B) || Keyboard::isKeyPressed(Keyboard::Key::Escape);
	
	if (!m_isNPCDialogueOpen && m_activeNPCId >= 0)
	{
		if (interactPressed) {
			m_npcInputBuffer.clear();
			m_npcLastResponse.clear();
			m_isNPCDialogueOpen = true;
			m_currentDialogueOptions = { "Show me what you've got.", "Maybe later." };
			m_selectedResponse = 0;
		}
		return;
	}


	if (m_isNPCDialogueOpen)
	{
		if (cancelPressed) {
			m_isNPCDialogueOpen = false;
			m_currentDialogueOptions.clear();
			m_selectedResponse = -1;

			m_npcInputBuffer.clear();
		}

		return;
	}

	// Shop Interactions
	const sf::FloatRect weaponShopBounds = m_weaponShop.getGlobalBounds();
	const sf::FloatRect cosmeticShopBounds = m_cosmeticShop.getGlobalBounds();
	const sf::FloatRect armoryShopBounds = m_armoryShop.getGlobalBounds();
	const sf::FloatRect playerShopBounds = m_playerShop.getGlobalBounds();
	const sf::FloatRect jobBoardBounds = m_jobBoard.getGlobalBounds();

	if (jobBoardBounds.findIntersection(playerBounds))
		m_isAtJobBoard = true;
	else if (weaponShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::WEAPON_SHOP;
	else if (cosmeticShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::COSMETIC_SHOP;
	else if (armoryShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::ARMORY_SHOP;
	else if (playerShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::PLAYER_SHOP;

	if (m_isAtJobBoard)
	{
		const bool interactPressed = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
		if (!interactPressed || m_blockJobBoardInteract)
			return;

		m_menuUI.setQuestBoardQuests(m_questManager.getBoardQuests());
		m_menuUI.setScreen(MenuScreen::QUEST_BOARD_SCREEN);
		m_blockJobBoardInteract = true; // Prevent multiple openings of the quest board while in range
	}

	switch (m_activeShop)
	{
	case HubShopType::WEAPON_SHOP: {
		const bool WeaponUpgradeInput = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
		const bool AmmoUpgradeInput = InputManager::pad().pressed(GamepadButton::B) || Keyboard::isKeyPressed(Keyboard::Key::Enter);

		if (!WeaponUpgradeInput && !AmmoUpgradeInput)
			return;

		const int upgradeCost = 100;

		// Weapon Upgrades
		if (WeaponUpgradeInput && m_coins >= upgradeCost)
		{
			const auto& loadout = m_player.getWeaponLoadout();
			const int currentSlot = m_player.getCurrentWeaponID();

			const WeaponType selectedType = loadout[currentSlot].type;
			int* selectedUpgradeLevel = nullptr;
			std::string weaponName = "";

			if (selectedType >= WeaponType::KNIFE)
				break;

			switch (selectedType)
			{
			case WeaponType::PISTOL:
				selectedUpgradeLevel = &m_pistolUpgradeLevel;
				weaponName = "Pistol";
				break;

			case WeaponType::ASSAULT_RIFLE:
				selectedUpgradeLevel = &m_arUpgradeLevel;
				weaponName = "Assault Rifle";
				break;

			case WeaponType::SHOTGUN:
				selectedUpgradeLevel = &m_shotgunUpgradeLevel;
				weaponName = "Shotgun";
				break;
			}

			m_coins -= upgradeCost;
			(*selectedUpgradeLevel)++;
			WeaponInLoadout droppedWeapon;
			m_player.swapCurrentWeapon(selectedType, *selectedUpgradeLevel, droppedWeapon);
			std::cout << weaponName << " upgraded to level " << *selectedUpgradeLevel << "!\n"
				<< "Coins remaining: " << m_coins << "\n";
		}

		// Ammo Upgrades
		else if (AmmoUpgradeInput && m_coins >= upgradeCost)
		{
			m_coins -= upgradeCost;
			m_playerAmmoUpgradeLevel++;
			m_player.applyUpgrade(m_playerHealthUpgradeLevel, m_playerSpeedUpgradeLevel, m_playerAmmoUpgradeLevel);
			std::cout << "Player ammo capacity upgraded to level " << m_playerAmmoUpgradeLevel << "!\n"
				<< "Coins remaining: " << m_coins << "\n";
		}

		// Not enough coins
		else
		{
			std::cout << "Not enough coins to upgrade weapon or ammo! Coins: " << m_coins << "\n";
		}

		break;
	}

	case HubShopType::COSMETIC_SHOP: {
		const bool interactPressed = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
		if (!interactPressed)
			return;

		m_playerColorIndex = (m_playerColorIndex + 1) % m_playerColorOptions.size();
		m_player.setBodyColor(m_playerColorOptions[m_playerColorIndex]);
		std::cout << "Player color changed to " << m_playerColorIndex << "!\n";
		break;
	}

	case HubShopType::ARMORY_SHOP: {
		const bool interactPressed = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
		if (!interactPressed)
			return;

		const WeaponType weaponToEquip = m_armoryCatalog[m_armoryCatalogIndex];

		int equipLevel = 1;
		switch (weaponToEquip)
		{
		case WeaponType::PISTOL:
			equipLevel = m_pistolUpgradeLevel;
			break;
		case WeaponType::ASSAULT_RIFLE:
			equipLevel = m_arUpgradeLevel;
			break;
		case WeaponType::SHOTGUN:
			equipLevel = m_shotgunUpgradeLevel;
			break;
		}

		WeaponInLoadout droppedWeapon;
		if (m_player.swapCurrentWeapon(weaponToEquip, equipLevel, droppedWeapon))
		{
			std::cout << "Equipped " << weaponToEquip << " at level " << equipLevel << "!\n";
			if (droppedWeapon.type != WeaponType::COUNT)
				std::cout << "Dropped " << droppedWeapon.type << " at level " << droppedWeapon.level << ".\n";
		}
		else
		{
			std::cout << "Failed to equip weapon. Loadout may be full.\n";
		}

		m_armoryCatalogIndex++;
		if (m_armoryCatalogIndex >= m_armoryCatalog.size())
			m_armoryCatalogIndex = 0;

		break;
	}

	case HubShopType::PLAYER_SHOP: {
		const bool healthUpgradeInput = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
		const bool speedUpgradeInput = InputManager::pad().pressed(GamepadButton::B) || Keyboard::isKeyPressed(Keyboard::Key::Enter);

		if (!healthUpgradeInput && !speedUpgradeInput)
			return;

		const int upgradeCost = 100;

		if (healthUpgradeInput && m_coins >= upgradeCost)
		{
			m_coins -= upgradeCost;
			m_playerHealthUpgradeLevel++;
			m_player.applyUpgrade(m_playerHealthUpgradeLevel, m_playerSpeedUpgradeLevel, m_playerAmmoUpgradeLevel);
			std::cout << "Player health upgraded! Coins remaining: " << m_coins << "\n";
		}
		else if (speedUpgradeInput && m_coins >= upgradeCost)
		{
			m_coins -= upgradeCost;
			m_playerSpeedUpgradeLevel++;
			m_player.applyUpgrade(m_playerHealthUpgradeLevel, m_playerSpeedUpgradeLevel, m_playerAmmoUpgradeLevel);
			std::cout << "Player speed upgraded! Coins remaining: " << m_coins << "\n";
		}
		else
		{
			std::cout << "Not enough coins to upgrade player! Coins: " << m_coins << "\n";
		}

		break;
	}
	}
}

/**
 * @brief Renders contextual prompts for hub interactions.
 */
void Game::renderHubShopPrompt()
{
	if (!m_isAtJobBoard &&
		m_activeShop == HubShopType::NONE_SHOP &&
		!m_isNPCDialogueOpen &&
		m_activeNPCId < 0)
		return;

	sf::Text promptText(m_arialFont);
	promptText.setCharacterSize(12);
	promptText.setOutlineThickness(1.f);
	promptText.setOutlineColor(sf::Color::Black);
	promptText.setFillColor(sf::Color::White);

	// NPC Dialogue
	auto shopTypeToString = [](HubShopType type) -> std::string
		{
			switch (type)
			{
			case HubShopType::WEAPON_SHOP: return "Weapon Shop";
			case HubShopType::COSMETIC_SHOP: return "Cosmetic Shop";
			case HubShopType::ARMORY_SHOP: return "Armory Shop";
			case HubShopType::PLAYER_SHOP: return "Player Shop";
			default: return "Unknown Shop";
			}
		};

	// Active dialogue box
	if (m_isNPCDialogueOpen && m_activeNPCId >= 0 && m_activeNPCId < m_hubNPCs.size())
	{
		const NPC& npc = m_hubNPCs[m_activeNPCId];
		const std::string option0Prefix = (m_selectedResponse == 0) ? "> " : "  ";
		const std::string option1Prefix = (m_selectedResponse == 1) ? "> " : "  ";

		std::string dialogue =
			npc.getInfo().name + " - " + shopTypeToString(npc.getInfo().shopType) + "\n\n" +
			"NPC: " + (m_npcLastResponse.empty() ? npc.getDialogue() : m_npcLastResponse) + "\n\n" +
			"You: " + (m_npcInputBuffer.empty() ? "_" : m_npcInputBuffer) + "\n\n" +
			"Type to chat | Enter = Send | B/Esc = Close";

		const sf::View previousView = m_window.getView();
		m_window.setView(m_window.getDefaultView());

		const sf::Vector2u windowSize = m_window.getSize();
		sf::RectangleShape dialogueBox;
		dialogueBox.setSize(sf::Vector2f(static_cast<float>(windowSize.x) - 40.f, 170.f));
		dialogueBox.setPosition(sf::Vector2f(20.f, static_cast<float>(windowSize.y) - 190.f));
		dialogueBox.setFillColor(sf::Color(0, 0, 0, 210));
		dialogueBox.setOutlineThickness(2.f);
		dialogueBox.setOutlineColor(sf::Color::White);
		m_window.draw(dialogueBox);

		promptText.setString(dialogue);
		promptText.setCharacterSize(16);
		promptText.setPosition(dialogueBox.getPosition() + sf::Vector2f(14.f, 12.f));
		m_window.draw(promptText);

		m_window.setView(previousView);
		return;
	}

	// Talk prompt
	if (m_activeNPCId >= 0 && m_activeNPCId < m_hubNPCs.size())
	{
		promptText.setString("Press A / Space to talk to " + m_hubNPCs[m_activeNPCId].getInfo().name);
		promptText.setOrigin(promptText.getGlobalBounds().getCenter());
		promptText.setPosition({ m_player.getPosition().x, m_player.getPosition().y - 50.f });
		m_window.draw(promptText);
		return;
	}


	// Job board and Shop
	if (m_isAtJobBoard)
	{
		promptText.setString("Press A / Space to open the Job Board");
	}
	else
	{
		switch (m_activeShop)
		{
		case HubShopType::WEAPON_SHOP:
			promptText.setString("Press A / Space to upgrade equipped weapon for 100 coins!\n Press B / Enter to Upgrade Ammo capacity for 100 coins");
			break;

		case HubShopType::COSMETIC_SHOP:
			promptText.setString("Press A / Space to change player color!");
			break;

		case HubShopType::ARMORY_SHOP:
			promptText.setString("Press A / Space to swap current weapon with one from the armory!");
			break;

		case HubShopType::PLAYER_SHOP:
			promptText.setString("Press A / Space to Upgrade Player Health for 100 coins\n Press B / Enter to Upgrade Player Speed for 100 coins");
			break;
		}
	}

	promptText.setOrigin(promptText.getGlobalBounds().getCenter());
	promptText.setPosition({ m_player.getPosition().x, m_player.getPosition().y - 50 });

	m_window.draw(promptText);
}

#pragma endregion


#pragma region Menu UI
/**
 * @brief Applies a single menu action produced by `MenuUI`.
 * @param action Action to apply.
 */
void Game::applyMenuAction(MenuAction action)
{
	switch (action)
	{
	case MenuAction::ACTION_NEW_GAME:
		startNewGame();
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;

	case MenuAction::ACTION_CONTINUE:
		m_gameMode = GameMode::HUB;
		enterHubWorld();
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;

	case MenuAction::ACTION_EXIT:
		m_window.close();
		break;

	case MenuAction::ACTION_RESUME:
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;

	case MenuAction::ACTION_HUB_WORLD:
		enterHubWorld();
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;

	case MenuAction::ACTION_MAIN_MENU:
		m_menuUI.setScreen(MenuScreen::MAIN_MENU_SCREEN);
		break;
	case MenuAction::ACTION_QUEST_0:
		m_questManager.acceptQuest(0);
		m_blockJobBoardInteract = true;
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;
	case MenuAction::ACTION_QUEST_1:
		m_questManager.acceptQuest(1);
		m_blockJobBoardInteract = true;
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;
	case MenuAction::ACTION_QUEST_2:
		m_questManager.acceptQuest(2);
		m_blockJobBoardInteract = true;
		m_menuUI.setScreen(MenuScreen::GAMEPLAY_SCREEN);
		break;

	default:
		break;
	}
}
#pragma endregion
