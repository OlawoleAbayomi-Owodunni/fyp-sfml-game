#include "Game.h"
#include "InputManager.h"

#include "GruntEnemy.h"
#include "TurretEnemy.h"

#include "CombatRoom.h"
#include "PortalRoom.h"
#include "SpawnRoom.h"

#include "RoomDoorUtils.h"

#include <iostream>
#include <string>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(sf::Vector2u(ScreenSize::s_width, ScreenSize::s_height), 32), "FYP: Dungeonish Crawler", sf::Style::Default)
{
	m_llmManager.initAsync("ASSETS/LLM/MODELS/Llama-3.2-1B-Instruct-Q4_K_M.gguf");

	srand(time(NULL));
	init();
}

////////////////////////////////////////////////////////////
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

	enterHubWorld();
}

////////////////////////////////////////////////////////////
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
void Game::processGameEvents(const sf::Event& event)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressed->scancode)
		{
		case sf::Keyboard::Scancode::Escape:
			m_window.close();
			break;

		case sf::Keyboard::Scancode::Num0:
			if (m_gameMode != GameMode::DUNGEON || m_activeRoomId < 0 || m_activeRoomId >= static_cast<int>(m_roomPlans.size()))
			{
				cout << "No active dungeon room for LLM prompt.\n";
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

		default:
			break;
		};
	}
}



////////////////////////////////////////////////////////////
void Game::update(float dt)
{
	const Vector2i mousePos = Mouse::getPosition(m_window);
	const Vector2f mousePosF = m_window.mapPixelToCoords(mousePos);

	InputManager::update();
	
	if (m_gameMode == GameMode::HUB)
	{
		updateHubShops();
	}

	if( m_gameMode == GameMode::DUNGEON && m_player.isDead())
	{
		cout << "Player has died. Restarting game...\n";
		enterHubWorld();
		return;
	}

	// Game manager controls
	ControllerInputHandler();
	updateActiveRoom();

	// Update player and enemies
	m_player.update(dt, mousePosF, m_gameProjectiles, m_activeDamageTriggers);

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
			enemy_it = m_enemies.erase(enemy_it);
		}
		else
			enemy_it++;
	}

	//Collision checks
	CollisionChecks();

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
			enterHubWorld();
		else
			loadNewFloor();
	}

	// LLM polling
	if (auto initOk = m_llmManager.tryConsumeInitResult())
	{
		if (*initOk) {
			cout << "LLM initialized successfully.\n";
		}
		else {
			cout << "LLM failed to initialize.\n";
			m_llmManager.initAsync("ASSETS/LLM/MODELS/Llama-3.2-1B-Instruct-Q4_K_M.gguf"); // Retry initialization
		}
	}

	if (auto response = m_llmManager.tryConsumeLatestResponse())
	{
		cout << "LLM Response: " << *response << "\n";
	}
}

////////////////////////////////////////////////////////////
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

	if (m_gameMode == GameMode::HUB)
	{
		m_window.draw(m_weaponShop);
		m_window.draw(m_cosmeticShop);
		m_window.draw(m_armoryShop);
		renderHubShopPrompt();
	}

#ifdef TEST_FPS
	m_window.draw(x_updateFPS); //ups is 60 and dps is 61
	m_window.draw(x_drawFPS);
#endif
	m_window.display();
}

///////////////////////////////////////////////////////////
#pragma region GAME MANAGEMENT
/// <summary>
/// Resets the game to its initial state, clearing all progress and reinitializing game systems.
/// </summary>
void Game::resetGame()
{
	m_playerCamera = m_window.getDefaultView();
	m_floorCamera = m_window.getDefaultView();
	m_floorCamera.zoom(12.f);
	m_gameCamera = m_playerCamera;
	m_isPlayerCamera = true;

	m_player.init();
	m_isInCombat = false;
	m_isInRoom = true;

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
}

/// <summary>
/// Initializes and starts a new game by generating a dungeon floor with rooms and their connections.
/// </summary>
void Game::gameStart()
{
	resetGame();
	m_gameMode = GameMode::DUNGEON;

	m_dungeonPlan.start(/*rand() % 50000*/12344); // using a fixed seed for testing purposes, can be randomized for more variety

	loadNewFloor();
}

void Game::enterHubWorld()
{
	resetGame();
	m_gameMode = GameMode::HUB;
	buildHubWorld();
	m_requestNextFloor = false;
}

void Game::startDungeonRun()
{
	gameStart();
}

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
	const sf::Vector2i cosmeticShopTile(3 * hubRoom.width / 4, hubRoom.height / 2);
	const sf::Vector2i armoryShopTile(hubRoom.width / 2, 3 * hubRoom.height / 4);

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

	// Initialize shops
	m_weaponShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_weaponShop.setOrigin(m_weaponShop.getSize() / 2.f);
	m_weaponShop.setPosition(gunShopCenter);
	m_weaponShop.setFillColor(sf::Color::Cyan);

	m_cosmeticShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_cosmeticShop.setOrigin(m_cosmeticShop.getSize() / 2.f);
	m_cosmeticShop.setPosition(cosmeticShopCenter);
	m_cosmeticShop.setFillColor(sf::Color::Magenta);

	m_armoryShop.setSize(sf::Vector2f(tileSize * 3, tileSize * 3));
	m_armoryShop.setOrigin(m_armoryShop.getSize() / 2.f);
	m_armoryShop.setPosition(armoryShopCenter);
	m_armoryShop.setFillColor(sf::Color::White);
}

void Game::updateHubShops()
{
	m_activeShop = HubShopType::NONE_SHOP;

	const sf::FloatRect playerBounds = m_player.getCollisionBounds();

	const sf::FloatRect weaponShopBounds = m_weaponShop.getGlobalBounds();
	const sf::FloatRect cosmeticShopBounds = m_cosmeticShop.getGlobalBounds();
	const sf::FloatRect armoryShopBounds = m_armoryShop.getGlobalBounds();

	if (weaponShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::WEAPON_SHOP;
	else if (cosmeticShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::COSMETIC_SHOP;
	else if (armoryShopBounds.findIntersection(playerBounds))
		m_activeShop = HubShopType::ARMORY_SHOP;

	const bool interactPressed = InputManager::pad().pressed(GamepadButton::A) || Keyboard::isKeyPressed(Keyboard::Key::Space);
	if (!interactPressed)
		return;

	switch (m_activeShop)
	{
	case HubShopType::WEAPON_SHOP: {
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

		default:
			cout << "Selected weapon unupgradable...\n";
		}

		const int upgradeCost = 100;
		if (m_coins >= upgradeCost)
		{
			m_coins -= upgradeCost;
			(*selectedUpgradeLevel)++;
			WeaponInLoadout droppedWeapon;
			m_player.swapCurrentWeapon(selectedType, *selectedUpgradeLevel, droppedWeapon);
			cout << weaponName << " upgraded to level " << *selectedUpgradeLevel << "!\n"
				<< "Coins remaining: " << m_coins << "\n";
		}
		else {
			cout << "Not enough coins to upgrade weapon! Coins: " << m_coins << "\n";
		}
		break;
	}

	case HubShopType::COSMETIC_SHOP:
		m_playerColorIndex = (m_playerColorIndex + 1) % m_playerColorOptions.size();
		m_player.setBodyColor(m_playerColorOptions[m_playerColorIndex]);
		cout << "Player color changed to " << m_playerColorIndex << "!\n";
		break;

	case HubShopType::ARMORY_SHOP: {
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
			cout << "Equipped " << weaponToEquip << " at level " << equipLevel << "!\n";
			if (droppedWeapon.type != WeaponType::COUNT)
				cout << "Dropped " << droppedWeapon.type << " at level " << droppedWeapon.level << ".\n";
		}
		else
		{
			cout << "Failed to equip weapon. Loadout may be full.\n";
		}

		m_armoryCatalogIndex++;
		if (m_armoryCatalogIndex >= m_armoryCatalog.size())
			m_armoryCatalogIndex = 0;

		break;
	}
	}
}

void Game::renderHubShopPrompt()
{
	if (m_activeShop == HubShopType::NONE_SHOP)
		return;

	sf::Text promptText(m_arialFont);
	promptText.setCharacterSize(24);
	promptText.setFillColor(sf::Color::Black);
	promptText.setOutlineColor(sf::Color::White);
	promptText.setOutlineThickness(1.f);
	promptText.setPosition({ m_player.getPosition().x - 50, m_player.getPosition().y - 50 });

	switch (m_activeShop)
	{
	case HubShopType::WEAPON_SHOP:
		promptText.setString("Press A / Space to upgrade weapon for 100 coins!");
		break;

	case HubShopType::COSMETIC_SHOP:
		promptText.setString("Press A / Space to change player color!");
		break;

	case HubShopType::ARMORY_SHOP:
		promptText.setString("Press A / Space to swap current weapon with one from the armory!");
		break;
	}

	m_window.draw(promptText);
}

/// <summary>
/// Manages wave-based combat by checking if all enemies are defeated and either starting a new wave or ending combat.
/// </summary>
void Game::ManageWave()
{
	// check if all enemies are dead
	if (m_enemies.empty()) {
		m_waveCounter--;
		if (m_waveCounter > 0) {	// start new wave
			cout << "New Wave Starting! Remaining Waves: " << m_waveCounter << endl;
			m_roomPlans[m_activeRoomId] = CombatRoom().generateNewWave(m_roomPlans[m_activeRoomId]);
			spawnEnemies(m_activeRoomId);
			generateRoom(m_activeRoomId);
		}
		else {	// end combat
			cout << "Combat Room Cleared at ID " << m_activeRoomId << "!\n";
			m_roomPlans[m_activeRoomId] = CombatRoom().setRoomCleared(m_roomPlans[m_activeRoomId]);
			generateRoom(m_activeRoomId);
			m_isInCombat = false;
		}
	}
}

#pragma endregion


////////////////////////////////////////////////////////////
#pragma region UPDATE SUBFUNCTIONS
/// <summary>
/// Performs collision detection and response for all game entities including enemies, projectiles, the player, and environment objects.
/// </summary>
void Game::CollisionChecks()
{	
	// ------------------- ENEMY CHECKS --------------------
	for (auto& enemy : m_enemies)
	{
		// Enemy -> Player
		if (CollisionCheck::areColliding(m_player, *enemy)) {
			cout << "Player collided with enemy!\n";
			
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
							cout << "Starting combat room wave!\nWaves Remaining: " << m_waveCounter << "\n";
							generateRoom(m_activeRoomId);
						}
					}
					break;
				}
			}

		}
	}
}

/// <summary>
/// Updates the active room ID based on the player's current position by checking which room bounds contain the player's center point.
/// </summary>
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

/// <summary>
/// Handles game input for controlling game state, including closing the window and restarting the game.
/// </summary>
void Game::ControllerInputHandler()
{
	if (InputManager::pad().pressed(GamepadButton::Start))
	{
		m_window.close();
	}
	if (InputManager::pad().pressed(GamepadButton::Select))
	{
		if (m_gameMode == GameMode::HUB)
			startDungeonRun();
		else
			gameStart();
	}
	if (InputManager::pad().pressed(GamepadButton::DPadDown))
	{
		m_isPlayerCamera = !m_isPlayerCamera;
	}
	if (InputManager::pad().pressed(GamepadButton::DPadUp))
	{
		if (m_gameMode != GameMode::DUNGEON || m_activeRoomId < 0 || m_activeRoomId >= static_cast<int>(m_roomPlans.size()))
		{
			cout << "No active dungeon room for LLM prompt.\n";
			return;
		}
		LLM_GenerateRoomInfo();
	}
}

#pragma endregion


///////////////////////////////////////////////////////////
#pragma region ROOM MANAGEMENT
/// <summary>
/// Generates a room instance from a room plan at a specific world position.
/// </summary>
/// <param name="roomId">The identifier of the room to generate.</param>
void Game::generateRoom(int roomId)
{
	RoomPlan& roomPlan = m_roomPlans[roomId];
	Vector2f roomWorldPos = m_roomWorldPositions[roomId];
	m_roomInstances[roomId].buildFromPlan(roomPlan, roomWorldPos);
}

/// <summary>
/// Spawns the player at the designated player spawn point within the specified room.
/// </summary>
/// <param name="roomId">The identifier of the room where the player should be spawned.</param>
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

/// <summary>
/// Spawns enemies in a combat room at designated spawn points.
/// </summary>
/// <param name="roomId">The ID of the room in which to spawn enemies.</param>
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

#pragma endregion


///////////////////////////////////////////////
#pragma	region FLOOR MANAGEMENT
/// <summary>
/// Builds the floor instance by calculating room positions in world space and instantiating all rooms from their plans.
/// </summary>
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

/// <summary>
/// Builds corridor room instances connecting adjacent rooms based on the floor plan edges and room door positions.
/// </summary>
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

/// <summary>
/// Loads and initializes a new dungeon floor, generating its layout, rooms, and connections.
/// </summary>
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

	m_floorCamera.setCenter(m_roomWorldPositions[0]);
}

#pragma endregion


/////////////////////////////////////////////////////////////////

void Game::LLM_GenerateRoomInfo()
{
	if (!m_llmManager.isReady()) {
		cout << "LLM model is not ready.\n";
	}
	else {
		// LLM Prompt/Response test
		// Request generation of p
		if (!m_llmManager.isBusy())
		{
			// Generate Prompt
			RoomPlan currRoom = m_roomPlans[m_activeRoomId];

			std::string roomTypeStr;
			switch (currRoom.type) {
			case RoomType::CORRIDOR:
				roomTypeStr = "Corridor";
				break;
			case RoomType::SPAWN:
				roomTypeStr = "Spawn Room";
				break;
			case RoomType::PORTAL:
				roomTypeStr = "Portal Room";
				break;
			case RoomType::COMBAT:
				roomTypeStr = "Combat Room";
				break;
			}

			std::string clearedStr = currRoom.isCleared ? "Yes" : "No";

			cout << "Generating response...\n";
			const std::string prompt = "Generate a 2 sentence description of the room the player is in currently based on the following properties: Room Type: " + roomTypeStr
				+ ", Is Room Cleared: " + clearedStr
				+ ", Room Height: " + to_string(currRoom.height)
				+ ", Room Width: " + to_string(currRoom.width)
				+ ", Dungeon Floor: " + to_string(m_dungeonPlan.currentFloorId) + "\n";

			cout << "Prompt: " << prompt << "\n";

			const bool queued = m_llmManager.requestGenerate(prompt);
			if (queued)
				cout << "Generation request queued successfully.\n";
			else
				cout << "Failed to queue generation request.\n";
		}
		else
		{
			cout << "Generation in progress, please wait...\n";
		}
	}
}
