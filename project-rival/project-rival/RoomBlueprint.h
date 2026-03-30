#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <algorithm>

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

enum Tile {
	Empty,

	FLOOR,
	WALL,
	DOOR
};

enum DoorDirection {
	NORTH,
	SOUTH,
	EAST,
	WEST
};

struct DoorPlan {
	sf::Vector2i tileStartPos;
	DoorDirection direction;

	bool isLocked;
	int spanTiles; // st = 2 if width/height % 2 = 0, else st = 3 {if l/w is even, st = 2. if odd, st = 3}
};

enum SpawnerType {
	PlayerSpawner,
	EnemySpawner,
	PortalSpawner
};

struct SpawnerPlan {
	SpawnerType type;
	sf::Vector2i tilePos;
};

enum TriggerType {
	None,
	DoorTrigger,
	PortalTrigger
};

struct TriggerPlan {
	TriggerType type;
	sf::Vector2i tilePos;
};

struct RoomPlan {
	// Room metadata
	int id;
	RoomType type;
	int seed;	// for stuctured random generation of room layout and contents

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
	
	bool isValid() const {
		if (width > 0 && height > 0 && (tileMap.size() == width * height))
			return true;
		else
			return false;
	}

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