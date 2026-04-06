#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <algorithm>

/// <summary>
/// Different types of Rooms.
/// </summary>
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

/// <summary>
/// Different types of Tiles
/// </summary>
enum Tile {
	Empty,

	FLOOR,
	WALL,
	DOOR
};

/// <summary>
/// Specifies the direction of a door.
/// </summary>
enum DoorDirection {
	NORTH,
	SOUTH,
	EAST,
	WEST
};

/// <summary>
/// Represents the configuration and properties of a door in the game world.
/// </summary>
struct DoorPlan {
	sf::Vector2i tileStartPos;
	DoorDirection direction;

	bool isLocked;
	int spanTiles;
};

/// <summary>
/// Defines the types of spawners available in the game.
/// </summary>
enum SpawnerType {
	PlayerSpawner,
	EnemySpawner,
	PortalSpawner
};

/// <summary>
/// Represents a plan for spawning an entity, containing its type and tile position.
/// </summary>
struct SpawnerPlan {
	SpawnerType type;
	sf::Vector2i tilePos;
};

/// <summary>
/// Defines the types of triggers that can be used in the system.
/// </summary>
enum TriggerType {
	None,
	DoorTrigger,
	PortalTrigger
};

/// <summary>
/// Represents a plan for triggering an event at a specific tile location.
/// </summary>
struct TriggerPlan {
	TriggerType type;
	sf::Vector2i tilePos;
};

/// <summary>
/// Represents the blueprint and state of a room in the game, including its layout, dimensions, and contents.
/// </summary>
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
	
	/// <summary>
	/// Checks whether the object is in a valid state.
	/// </summary>
	/// <returns>True if the width and height are positive and the tile map size matches the dimensions; otherwise, false.</returns>
	bool isValid() const {
		if (width > 0 && height > 0 && (tileMap.size() == width * height))
			return true;
		else
			return false;
	}

	/// <summary>
	/// Retrieves the tile at the specified row and column position.
	/// </summary>
	/// <param name="row">The row index of the tile.</param>
	/// <param name="column">The column index of the tile.</param>
	/// <returns>The tile at the specified position.</returns>
	Tile getTile(int row, int column) const {
		return tileMap[row * width + column];
	}

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