#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <algorithm>

/**
 * @file RoomBlueprint.h
 * @brief Declares room/tile/door/spawner/trigger blueprint data used by procedural generation.
 */

/**
 * @brief High-level room types used by the dungeon generator.
 */
enum RoomType {
	CORRIDOR,
	SPAWN,
	PORTAL,
	//BOSS,
	//MINIBOSS,
	COMBAT,
	//TREASURE,
	//QUEST,

	ROOM_COUNT
};

/**
 * @brief Tile types used by room tile maps.
 */
enum Tile {
	Empty,

	FLOOR,
	WALL,
	DOOR
};

/**
 * @brief Cardinal direction a door faces.
 */
enum DoorDirection {
	NORTH,
	SOUTH,
	EAST,
	WEST
};

/**
 * @brief Door placement and properties within a room plan.
 */
struct DoorPlan {
	sf::Vector2i tileStartPos;
	DoorDirection direction;

	bool isLocked;
	int spanTiles;
};

/**
 * @brief Types of spawners that can be placed in a room plan.
 */
enum SpawnerType {
	PlayerSpawner,
	EnemySpawner,
	PortalSpawner
};

/**
 * @brief Spawner location and type.
 */
struct SpawnerPlan {
	SpawnerType type;
	sf::Vector2i tilePos;
};

/**
 * @brief Types of triggers that can be placed in a room plan.
 */
enum TriggerType {
	None,
	DoorTrigger,
	PortalTrigger
};

/**
 * @brief Trigger location and type.
 */
struct TriggerPlan {
	TriggerType type;
	sf::Vector2i tilePos;
};

/**
 * @brief Blueprint and runtime state for a room.
 */
struct RoomPlan {
	// Room metadata
	int id;
	RoomType type;
	int seed;

	// Room representation
	int width;
	int height;	
	float tileSize = 64.f;

	// Room layout
	std::vector<Tile> tileMap;	// formula for 1D index: row * width + column
	std::vector<DoorPlan> doors;
	std::vector<SpawnerPlan> spawners;
	std::vector<TriggerPlan> triggers;

	// Room state
	bool isCleared = false;
	
   /**
	 * @brief Checks whether the plan is in a valid state.
	 * @return True when dimensions are positive and the tile map size matches @c width*@c height.
	 */
	bool isValid() const {
		if (width > 0 && height > 0 && (tileMap.size() == width * height))
			return true;
		else
			return false;
	}

   /**
	 * @brief Retrieves the tile at a row/column.
	 * @param row Row index.
	 * @param column Column index.
	 * @return Tile at the given position.
	 */
	Tile getTile(int row, int column) const {
		return tileMap[row * width + column];
	}

  /**
	 * @brief Sets a tile at a row/column.
	 * @param row Row index.
	 * @param column Column index.
	 * @param type New tile type.
	 */
	void setTile(int row, int column, Tile type) {
		tileMap[row * width + column] = type;
	}
};

class IRoomGenerator
{
public:
	virtual ~IRoomGenerator() = default;

	virtual RoomPlan generateRoomPlan(int id, RoomType type, int seed) = 0;

protected:
	virtual void generateSpawnPoints(RoomPlan& roomPlan, int interiorArea) = 0;

};