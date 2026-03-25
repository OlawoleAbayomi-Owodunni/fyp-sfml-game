#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <algorithm>

enum RoomType {
	SPAWN,
	PORTAL,
	COMBAT
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
	sf::Vector2i tilePos;
	DoorDirection direction;

	bool isLocked;
};

enum SpawnerType {
	PlayerSpawner,
	EnemySpawner
};

struct SpawnerPlan {
	SpawnerType type;
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

	// a few things to note about the parameters:
	// id, type and seed are fine for passing through
	virtual RoomPlan generateRoom(int id, RoomType type, int seed) = 0;
	virtual void generateSpawnPoints(RoomPlan& roomPlan, int interiorArea) = 0;
};