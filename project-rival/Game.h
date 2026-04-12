#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <string>
#include <queue>

#include "player.h"
#include "Enemy.h"
#include "NPC.h"

#include "Projectile.h"
#include "DamageTrigger.h"

#include "RoomInstance.h"
#include "FloorGenerator.h"
#include "FloorLayout.h"
#include "DungeonPlan.h"

#include "HubShop.h"

#include "Pickup.h"
#include "Chest.h"
#include "Quest.h"

#include "LLMService.h"

#include "PlayerHUD.h"
#include "MenuUI.h"

using namespace std;
using namespace sf;

/// <summary>
/// @author OA-O
/// @date November 2025
/// @version 1.0
/// 
/// </summary>

enum GameMode
{
	HUB,
	DUNGEON,

	MODE_COUNT
};

enum LLMJobType
{
	ROOM_DESCRIPTION,
	NPC_REPLY,
	QUEST_METADATA,

	LLM_JOB_TYPE_COUNT
};

struct LLMJobContext
{
	LLMJobType jobType;
	int npcId;
	int questIndex;
	std::string prompt;
};

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
	void ControllerInputHandler();

	// Game management
	void startNewGame();
	void resetGame();
	void gameStart();
	void enterHubWorld();
	void startDungeonRun();

	// UI management
	#pragma region Menu UI
	void applyMenuAction(MenuAction action);
	#pragma endregion

	// Hub management
	void buildHubWorld();
	void updateHubShops();
	void renderHubShopPrompt();

	// Room management
	void spawnPlayer(const int roomId);
	void spawnEnemies(const int roomId);
	void generateRoom(int roomId);

	// Floor Management
	void buildFloorInstance();
	void buildCorridors();
	void loadNewFloor();

	// Spawnable Management
	void spawnRandomPickup(const sf::Vector2f& worldPos, float tilesize, bool isFromChest);
	void trySpawnPickup(const sf::Vector2f& worldPos, float tilesize, int spawnChance);

	// LLM management
	void LLM_GenerateRoomInfo();
	void enqueLLMJob(LLMJobType jobType, int npcId, int questIndex, const std::string& prompt);
	void processLLMQueue();
	void handleLLMResponse(const std::string& response);

	// ----------------------------------> VARIABLES <---------------------------------- //	
	// Game Management
	sf::View m_gameCamera;
	bool m_isPlayerCamera;
	GameMode m_gameMode;

	#pragma region Menu UI
	MenuUI m_menuUI{ m_arialFont, m_gameCamera };
	#pragma endregion

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

	// Spawnable management
	std::vector<std::unique_ptr<Pickup>> m_gamePickups;
	float m_playerPickupDropTimer = 0.f;
	std::vector<std::unique_ptr<Chest>> m_gameChests;

	// NPC management
	std::vector<NPC> m_hubNPCs;
	int m_activeNPCId;
	bool m_isNPCDialogueOpen;
	int m_selectedResponse;
	std::vector<std::string> m_currentDialogueOptions;

	// Hub management
	sf::RectangleShape m_weaponShop;
	sf::RectangleShape m_cosmeticShop;
	sf::RectangleShape m_armoryShop;
	sf::RectangleShape m_playerShop;
	sf::RectangleShape m_jobBoard;
	HubShopType m_activeShop;
	bool m_isAtJobBoard;
	bool m_blockJobBoardInteract{ false };

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
	bool m_hasActiveLLMJob;
	LLMService m_llmManager;
	std::queue <LLMJobContext> m_llmJobQueue;
	LLMJobContext m_currentLLMJob;

	std::string m_latestRoomDescription;
	float m_roomDescriptionTtl = 0.f;
	sf::Text m_roomDescriptionText{ m_arialFont };

	// Economy, Upgrades & Progression
	int m_coins = 500;

	int m_pistolUpgradeLevel = 1;
	int m_arUpgradeLevel = 1;
	int m_shotgunUpgradeLevel = 1;

	std::vector<WeaponType> m_armoryCatalog{
		WeaponType::PISTOL,
		WeaponType::ASSAULT_RIFLE,
		WeaponType::SHOTGUN,
		WeaponType::KNIFE,
		WeaponType::SWORD,
		WeaponType::AXE
	};
	int m_armoryCatalogIndex;

	int m_playerHealthUpgradeLevel = 1;
	int m_playerSpeedUpgradeLevel = 1;
	int m_playerAmmoUpgradeLevel = 1;

	std::vector<sf::Color> m_playerColorOptions{
		sf::Color::Green,
		sf::Color::Cyan,
		sf::Color::Magenta,
		sf::Color::Yellow
	};
	int m_playerColorIndex;

	// UI management
	PlayerHUD m_hud{ m_arialFont };

	#pragma region Quest System
	// Quest management
	QuestManager m_questManager;
	#pragma endregion
};
