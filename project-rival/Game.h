#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <string>

#include "player.h"
#include "Enemy.h"

#include "Projectile.h"
#include "DamageTrigger.h"

#include "RoomInstance.h"
#include "FloorGenerator.h"
#include "FloorLayout.h"
#include "DungeonPlan.h"

#include "LLMService.h"

using namespace std;
using namespace sf;

/// <summary>
/// @author OA-O
/// @date November 2025
/// @version 1.0
/// 
/// </summary>

struct ScreenSize
{
public:
	static const int s_width{ 1440 };

	static const int s_height{ 900 };
};

class Game
{
public:
	Game();
	void run();

protected:
	void init();
	void update(float dt);

	void ManageWave();

	void updateActiveRoom();

	void render();

	void processEvents();
	void processGameEvents(const sf::Event&);

	sf::Font m_arialFont{ "ASSETS/FONTS/ariblk.ttf" };
	sf::RenderWindow m_window;

#ifdef TEST_FPS
	sf::Text x_updateFPS{ m_arialFont };	// text used to display updates per second.
	sf::Text x_drawFPS{ m_arialFont };	// text used to display draw calls per second.
	sf::Time x_secondTime{ sf::Time::Zero };			// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };						// updates per second counter.
	int x_drawFrameCount{ 0 };							// draws per second counter.
#endif // TEST_FPS

private:
	// ----------------------------------> FUNCTIONS <---------------------------------- //
	// Update subfunctions
	void CollisionChecks();
	void gameInput();

	// Game management
	void resetGame();
	void gameStart();

	// Room management
	void spawnPlayer(const int roomId);
	void spawnEnemies(const int roomId);
	void generateRoom(int roomId);

	// Floor Management
	void buildFloorInstance();
	void buildCorridors();
	void loadNewFloor();


	// ----------------------------------> VARIABLES <---------------------------------- //	
	// Game Management
	sf::View m_gameCamera;
	bool m_isPlayerCamera;

	// Player management
	Player m_player;
	bool m_isInCombat;
	sf::View m_playerCamera;
	bool m_isInRoom;

	// Enemy management
	std::vector<std::unique_ptr<Enemy>> m_enemies;

	// Bullet management
	std::vector<std::unique_ptr<Projectile>> m_gameProjectiles;

	// Instantiable trigger management
	std::vector<std::unique_ptr<DamageTrigger>> m_activeDamageTriggers;

	// Room management
	vector<RoomPlan> m_roomPlans;
	vector<RoomInstance> m_roomInstances;
	vector<Vector2f> m_roomWorldPositions;
	int m_activeRoomId;

	// Floor management
	sf::View m_floorCamera;
	FloorPlan m_floorPlan;
	FloorGenerator m_floorGenerator;

	FloorLayout m_floorLayout;
	FloorLayoutGenerator m_floorInstanceGenerator;

	// Dungeon management
	DungeonPlan m_dungeonPlan;
	bool m_requestNextFloor;

	// Wave management
	int m_waveCounter;

	// LLM management
	LLMService m_llmManager;
};
