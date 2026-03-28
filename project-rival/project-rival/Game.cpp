#include "Game.h"
#include "InputManager.h"
#include "GruntEnemy.h"
#include "TurretEnemy.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(sf::Vector2u(ScreenSize::s_width, ScreenSize::s_height), 32), "FYP: Dungeonish Crawler", sf::Style::Default)
{
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

	gameStart();
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
		case sf::Keyboard::Scancode::Up:
			// Up key was pressed...
			break;
		default:
			break;
		}
	}
}


////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	const Vector2i mousePos = Mouse::getPosition(m_window);
	const Vector2f mousePosF = m_window.mapPixelToCoords(mousePos);

	InputManager::update();
	
	// Game manager controls
	gameInput();
	// Wave management for combat rooms
	if (m_activeRoomPlan.type == RoomType::COMBAT && m_isInCombat) {
		// check if all enemies are dead
		if (m_enemies.empty()) {
			m_waveCounter--;
			if (m_waveCounter > 0) {	// start new wave
				cout << "New Wave Starting! Remaining Waves: " << m_waveCounter << endl;
				m_activeRoomPlan = CombatRoom().generateNewWave(m_activeRoomPlan);
				spawnEnemies(m_activeRoomPlan, m_roomWorldPos);
				generateRoom(m_activeRoomPlan);
			}
			else {	// end combat
				cout << "Combat Room Cleared at ID " << m_activeRoomPlan.id << "!\n";
				m_activeRoomPlan = CombatRoom().setRoomCleared(m_activeRoomPlan);
				generateRoom(m_activeRoomPlan);
				m_isInCombat = false;
			}
		}
	}


	// Update player and enemies
	Vector2f oldPlayerPos = m_player.getPosition();
	m_player.update(dt, mousePosF);

	for (auto enemy_it = m_enemies.begin(); enemy_it != m_enemies.end();)
	{
		auto& enemy = *enemy_it;
		//Vector2f oldEnemyPos = enemy->getPosition();
		enemy->setTarget(m_player.getPosition());
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
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));

	m_activeRoomInstance.render(m_window);

	m_player.render(m_window);
	for (auto& enemy : m_enemies) {
		enemy->render(m_window);
	}

#ifdef TEST_FPS
	m_window.draw(x_updateFPS); //ups is 60 and dps is 61
	m_window.draw(x_drawFPS);
#endif
	m_window.display();
}

void Game::resetGame()
{
	m_player.init();
	m_isInCombat = false;

	m_enemies.clear();

	m_activeRoomInstance.reset();
	m_roomPlans.clear();
	m_activeRoomPlan = RoomPlan();
}


////////////////////////////////////////////////////////////

void Game::CollisionChecks()
{
	//__________________________ COLLISION CHECKS __________________________//
	
	// ------------------- ENEMY CHECKS --------------------
	for (auto& enemy : m_enemies)
	{
		// Enemy -> Player
		if (CollisionCheck::areColliding(m_player, *enemy)) {
			cout << "Player collided with enemy!\n";
			m_player.takeDamage(1);
			break;
		}
	}

	// ------------------- BULLET CHECKS -------------------
	for (auto& bullet : m_player.getProjectiles())
	{
		if (bullet->shouldDestroy())
			continue;

		// Bullet -> Enemy
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
	}

	// -------------------- ENVIRONMENT CHECKS -------------------
	for (auto& collisionObject : m_activeRoomInstance.getStaticRoomColliders())
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

			// Wall/Door -> Player Projectiles
			for (auto& bullet : m_player.getProjectiles())
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
				cout << "Player Collided with Portal Trigger!\n";
				break;
			}
		}

		// Door trigger checks
		if (collisionObject.getCollisionProfile().layer == CollisionLayer::DOOR_TRIGGER_LAYER)
		{
			// PROBLEM -> we need a way to set the active room to be the room that the this collision happens in
			// SOLUTION -> 

			// Door Trigger -> Player
			if (CollisionCheck::areColliding(collisionObject, m_player))
			{
				// Combat Door Trigger
				if (m_activeRoomPlan.type == RoomType::COMBAT) {
					//cout << "Player collided with combat room door trigger!\n";
					if (!m_activeRoomPlan.isCleared && !m_isInCombat)
					{
						m_activeRoomPlan = CombatRoom().generateNewWave(m_activeRoomPlan);
						spawnEnemies(m_activeRoomPlan, m_roomWorldPos);
						m_isInCombat = true;
						m_waveCounter = rand() % 3 + 1;
						cout << "Starting combat room wave!\nWaves Remaining: " << m_waveCounter << "\n";
						generateRoom(m_activeRoomPlan);
					}
				}

				break;
			}
		}
	}
}

void Game::gameInput()
{
	if (Keyboard::isKeyPressed(Keyboard::Key::Escape) || InputManager::pad().pressed(GamepadButton::Start))
	{
		m_window.close();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::R) || InputManager::pad().pressed(GamepadButton::Select))
	{
		gameStart();
	}
}


///////////////////////////////////////////////////////////
void Game::gameStart()
{
	resetGame();
	m_roomPlans.push_back(CombatRoom().generateRoomPlan(0, RoomType::COMBAT, 0));
	m_roomPlans.push_back(SpawnRoom().generateRoomPlan(1, RoomType::SPAWN, 0));
	m_roomPlans.push_back(PortalRoom().generateRoomPlan(2, RoomType::PORTAL, 0));

	int roomToLoad = rand() % m_roomPlans.size();	// temportary for displaying single rooms at a time
	m_activeRoomPlan = m_roomPlans[0];
	generateRoom(m_activeRoomPlan);
}

void Game::generateRoom(RoomPlan& roomPlan)
{
	m_activeRoomInstance.buildFromPlan(roomPlan, m_roomWorldPos);
	spawnPlayer(roomPlan, m_roomWorldPos);
}


/////////////////////////////////////////////
void Game::spawnPlayer(RoomPlan& roomPlan, const sf::Vector2f& roomWorldPos)
{
	// Spawn room Spawner
	if (roomPlan.type == RoomType::SPAWN)
	{
		for (auto& spawnPoint : roomPlan.spawners)
		{
			if (spawnPoint.type == SpawnerType::PlayerSpawner) {
				Vector2f spawnPos = roomWorldPos + static_cast<Vector2f>(spawnPoint.tilePos) * roomPlan.tileSize;
				m_player.setSpawnPosition(spawnPos);
				m_activeRoomPlan = roomPlan;
			}
		}
	}
}

void Game::spawnEnemies(RoomPlan& roomPlan, const sf::Vector2f& roomWorldPos)
{
	// Combat room Spawner
	m_enemies.clear();
	if (roomPlan.type == RoomType::COMBAT)
	{
		int totalEnemyTypes = EnemyType::ENEMY_COUNT;
		for (auto& spawnPoint : roomPlan.spawners)
		{
			if (spawnPoint.type == SpawnerType::EnemySpawner) {
				Vector2f spawnPos = roomWorldPos + static_cast<Vector2f>(spawnPoint.tilePos) * roomPlan.tileSize;

				int enemyToSpawn = rand() % totalEnemyTypes;
				if (enemyToSpawn == 0)
					m_enemies.push_back(std::make_unique<GruntEnemy>(spawnPos, 150));
				else if (enemyToSpawn == 1)
					m_enemies.push_back(std::make_unique<TurretEnemy>(spawnPos, 250));
			}
		}
	}
}
