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
	virtual RoomPlan generateRoomPlan(int id, RoomType type, int seed) = 0;

protected:
	virtual void generateSpawnPoints(RoomPlan& roomPlan, int interiorArea) = 0;

	inline void generateDoors(RoomPlan& roomPlan, DoorDirection dir)
	{
		DoorPlan door;
		door.direction = dir;
		door.isLocked = false;

		// seeding starting tile variables
		int rowStart = 0;
		int colStart = 0;

		// determine door length and position based on direction
		if (door.direction == DoorDirection::NORTH || door.direction == DoorDirection::SOUTH)
		{
			// even width = 2 tiles, odd width = 3 tiles
			if (roomPlan.width % 2 == 0) door.spanTiles = 2;
			else door.spanTiles = 3;

			// determine doors starting tile position based on direction and span
			colStart = (roomPlan.width - door.spanTiles) / 2;
			int row;
			if (door.direction == DoorDirection::NORTH) row = 0;
			else if (door.direction == DoorDirection::SOUTH) row = roomPlan.height - 1;

			door.tileStartPos = { colStart, row };

			// set door tiles and triggers
			for (int i = 0; i < door.spanTiles; i++) {
				roomPlan.setTile(row, colStart + i, Tile::DOOR);
				roomPlan.triggers.push_back({ TriggerType::DoorTrigger, {colStart + i, row} });
			}
		}
		else if (door.direction == DoorDirection::EAST || door.direction == DoorDirection::WEST)
		{
			// even height = 2 tiles, odd height = 3 tiles
			if (roomPlan.height % 2 == 0) door.spanTiles = 2;
			else door.spanTiles = 3;

			// determine doors starting tile position based on direction and span
			rowStart = (roomPlan.height - door.spanTiles) / 2;
			int col;
			if (door.direction == DoorDirection::WEST) col = 0;
			else if (door.direction == DoorDirection::EAST) col = roomPlan.width - 1;

			door.tileStartPos = { col, rowStart };

			// set door tiles and triggers
			for (int i = 0; i < door.spanTiles; i++) {
				roomPlan.setTile(rowStart + i, col, Tile::DOOR);
				roomPlan.triggers.push_back({ TriggerType::DoorTrigger, {col, rowStart + i} });
			}
		}

		roomPlan.doors.push_back(door);
	}
};