#include "Game.h"
#include "InputManager.h"

#include "GruntEnemy.h"
#include "TurretEnemy.h"

#include "CombatRoom.h"
#include "PortalRoom.h"
#include "SpawnRoom.h"

#include "RoomDoorUtils.h"

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
	//// Wave management for combat rooms
	//if (m_activeRoomPlan.type == RoomType::COMBAT && m_isInCombat) {
	//	// check if all enemies are dead
	//	if (m_enemies.empty()) {
	//		m_waveCounter--;
	//		if (m_waveCounter > 0) {	// start new wave
	//			cout << "New Wave Starting! Remaining Waves: " << m_waveCounter << endl;
	//			m_activeRoomPlan = CombatRoom().generateNewWave(m_activeRoomPlan);
	//			spawnEnemies(m_activeRoomPlan, m_roomWorldPos);
	//			generateRoom(m_activeRoomPlan);
	//		}
	//		else {	// end combat
	//			cout << "Combat Room Cleared at ID " << m_activeRoomPlan.id << "!\n";
	//			m_activeRoomPlan = CombatRoom().setRoomCleared(m_activeRoomPlan);
	//			generateRoom(m_activeRoomPlan);
	//			m_isInCombat = false;
	//		}
	//	}
	//}


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

	m_playerCamera.setCenter(m_player.getPosition());
	m_window.setView(m_playerCamera);
	
	for (auto& corridorInstance : m_corridorInstances){
		corridorInstance.render(m_window);
	}

	for (auto& roomInstance : m_roomInstances) {
		roomInstance.render(m_window);
	}

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

///////////////////////////////////////////////////////////
#pragma region GAME MANAGEMENT
void Game::resetGame()
{
	m_player.init();
	m_isInCombat = false;
	m_playerCamera = m_window.getDefaultView();

	m_enemies.clear();

	m_roomPlans.clear();
	m_roomInstances.clear();
	m_roomWorldPositions.clear();
	m_activeRoomId = 0;

	m_floorPlan = FloorPlan();
	m_floorGenerator = FloorGenerator();
	m_floorLayout = FloorLayout();
	m_floorInstanceGenerator = FloorLayoutGenerator();

	m_waveCounter = 0;
}

void Game::gameStart()
{
	resetGame();

	const int dungeonSeed = 12345;	// temporary seed for testing
	const int floorId = 0;
	const int roomCount = 10;

	m_floorPlan = m_floorGenerator.generateFloorPlan(floorId, dungeonSeed, roomCount, false);

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

	m_activeRoomId = 0;

}

#pragma endregion


////////////////////////////////////////////////////////////
#pragma region UPDATE SUBFUNCTIONS
void Game::CollisionChecks()
{	
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
					//// Combat Door Trigger
					//if (m_activeRoomPlan.type == RoomType::COMBAT) {
					//	//cout << "Player collided with combat room door trigger!\n";
					//	if (!m_activeRoomPlan.isCleared && !m_isInCombat)
					//	{
					//		m_activeRoomPlan = CombatRoom().generateNewWave(m_activeRoomPlan);
					//		spawnEnemies(m_activeRoomPlan, m_roomWorldPos);
					//		m_isInCombat = true;
					//		m_waveCounter = rand() % 3 + 1;
					//		cout << "Starting combat room wave!\nWaves Remaining: " << m_waveCounter << "\n";
					//		generateRoom(m_activeRoomPlan);
					//	}
					//}

					break;
				}
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
#pragma endregion


///////////////////////////////////////////////////////////
#pragma region ROOM MANAGEMENT
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

#pragma endregion


///////////////////////////////////////////////
#pragma	region FLOOR MANAGEMENT
/// <summary>
/// Builds the floor instance by calculating world positions for all rooms in a grid layout and instantiating them from their plans.
/// </summary>
void Game::buildFloorInstance()
{
	const int roomCount = m_floorPlan.rooms.size();
	if (roomCount <= 0) return;

	// Calculate the world size of each cell in the grid layout based on the largest room dimensions and a fixed gap between rooms
	const float tileSize = m_roomPlans[0].tileSize;
	const int gapTiles = 2;

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
		m_roomInstances[roomId].buildFromPlan(m_roomPlans[roomId], worldPos);

		// Spawn player in spawn room
		if (m_roomPlans[roomId].type == RoomType::SPAWN)
			spawnPlayer(m_roomPlans[roomId], worldPos);
	}

	buildCorridors();
}

/// <summary>
/// Generates and builds corridor connections between rooms in the game level. Clears existing corridor data, then creates corridor plans and instances for each edge in the floor plan by connecting the doors of adjacent rooms with either horizontal or vertical passages.
/// </summary>
void Game::buildCorridors()
{
	m_corridorPlans.clear();
	m_corridorInstances.clear();
	m_corridorWorldPositions.clear();

	if (m_roomPlans.empty()) return;

	const float tileSize = m_roomPlans[0].tileSize;

#pragma region Corridor Helper Lambdas
	auto getDoor = [&](const RoomPlan& roomPlan, DoorDirection dir)
		{
			for (auto& door : roomPlan.doors)
				if (door.direction == dir)
					return &door;
		};

	// Returns the tile position of the center of the door in the given direction for the specified room plan
	auto getDoorCenterTile = [&](const RoomPlan& roomPlan, DoorDirection dir)	
		{
			for (auto& door : roomPlan.doors)
			{
				if (door.direction != dir) continue;

				if (dir == DoorDirection::NORTH || dir == DoorDirection::SOUTH)
					return Vector2i(door.tileStartPos.x + door.spanTiles / 2, door.tileStartPos.y);
				else
					return Vector2i(door.tileStartPos.x, door.tileStartPos.y + door.spanTiles / 2);
			}
		};

	// Converts a tile position within a room to a world position based on the room's world position and tile size
	auto tileToWorldPos = [&](int roomId, Vector2i tilePos)
		{
			return m_roomWorldPositions[roomId] + Vector2f(static_cast<Vector2f>(tilePos) * tileSize);
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

	// Returns the world offset from the center of a door to the edge of the room in the given direction, used to position corridors just outside the doors
	auto dirToOffset = [&](DoorDirection dir)
		{
			switch (dir)
			{
			case DoorDirection::NORTH: return Vector2f(0, -tileSize);
			case DoorDirection::SOUTH: return Vector2f(0, tileSize);
			case DoorDirection::EAST: return Vector2f(tileSize, 0);
			case DoorDirection::WEST: return Vector2f(-tileSize, 0);
			}
		};
#pragma endregion

	int corridorIdCounter = 0;

	
	for (const auto& edge : m_floorPlan.edges)
	{
		const int aId = edge.id_a;
		const int bId = edge.id_b;

		const DoorDirection dirFromAtoB = directionBetweenRooms(aId, bId);
		const DoorDirection dirFromBtoA = opposite(dirFromAtoB);

		const Vector2i aDoorTile = getDoorCenterTile(m_roomPlans[aId], dirFromAtoB);
		const Vector2i bDoorTile = getDoorCenterTile(m_roomPlans[bId], dirFromBtoA);

		const Vector2f aDoorWorldPos = tileToWorldPos(aId, aDoorTile);
		const Vector2f bDoorWorldPos = tileToWorldPos(bId, bDoorTile);

		Vector2f start = aDoorWorldPos + dirToOffset(dirFromAtoB);
		Vector2f end = bDoorWorldPos + dirToOffset(dirFromBtoA);

		const bool isHorizontal = (dirFromAtoB == DoorDirection::EAST || dirFromAtoB == DoorDirection::WEST);

		// Create corridor plan and instance
		RoomPlan corridor;
		corridor.id = corridorIdCounter++;
		corridor.type = RoomType::CORRIDOR;
		corridor.seed = 0;
		corridor.tileSize = tileSize;

		Vector2f corridorWorldPos;

		if (isHorizontal)
		{
			if (end.x < start.x) std::swap(start, end);	// ensure start is always left of end for horizontal corridors
			const int lengthTiles = static_cast<int>((end.x - start.x) / tileSize);
			
			corridor.width = lengthTiles;
			corridor.height = 5;

			corridor.tileMap.assign(corridor.width * corridor.height, Tile::FLOOR);
			for (int i = 0; i < corridor.width; i++)
				for (int j = 0; j < corridor.height; j++)
					if (j == 0 || j == corridor.height - 1)
						corridor.tileMap[j * corridor.width + i] = Tile::WALL;

			corridorWorldPos = start - Vector2f(0, tileSize);
		}
		else
		{
			if (end.y < start.y) std::swap(start, end);	// ensure start is always above end for vertical corridors
			const int lengthTiles = static_cast<int>((end.y - start.y) / tileSize);

			corridor.width = 3;
			corridor.height = lengthTiles;

			corridor.tileMap.assign(corridor.width* corridor.height, Tile::FLOOR);
			for (int i = 0; i < corridor.width; i++)
				for (int j = 0; j < corridor.height; j++)
					if (i == 0 || i == corridor.width - 1)
						corridor.tileMap[j * corridor.width + i] = Tile::WALL;

			corridorWorldPos = start - Vector2f(tileSize, 0);
		}

		m_corridorPlans.push_back(corridor);
		m_corridorWorldPositions.push_back(corridorWorldPos);

		RoomInstance corridorInst;
		corridorInst.buildFromPlan(m_corridorPlans.back(), corridorWorldPos);
		m_corridorInstances.push_back(corridorInst);
	}

}

#pragma endregion
