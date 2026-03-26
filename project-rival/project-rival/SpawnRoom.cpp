#include "SpawnRoom.h"

RoomPlan SpawnRoom::generateRoom(int id, RoomType type, int seed)
{
	RoomPlan room;
	// Meta data
	room.id = id;
	room.type = RoomType::SPAWN;
	room.seed = seed;
	room.height = 11;
	room.width = 11;

	// tile map initialisation
	room.tileMap.assign(room.height * room.width, Tile::FLOOR);

	// tile map construction
	// outer walls
	for (int col = 0; col < room.width; col++)
	{
		room.setTile(0, col, Tile::WALL);				// top row
		room.setTile(room.height - 1, col, Tile::WALL);	// bottom row
	}
	for (int row = 0; row < room.height; row++)
	{
		room.setTile(row, 0, Tile::WALL);				// left column
		room.setTile(row, room.width - 1, Tile::WALL);	// right column
	}

	const int interiorWidth = room.width - 2;
	const int interiorHeight = room.height - 2;
	const int interiorArea = interiorWidth * interiorHeight;

	// Spawners
	generateSpawnPoints(room, interiorArea);

	// Doors
	generateDoors(room, DoorDirection::SOUTH);
	generateDoors(room, DoorDirection::EAST);

	return room;
}

void SpawnRoom::generateSpawnPoints(RoomPlan& roomPlan, int interiorArea)
{
	// Player Spawner
	sf::Vector2i roomCenter = sf::Vector2i(roomPlan.width, roomPlan.height) / 2;
	roomPlan.spawners.push_back({ SpawnerType::PlayerSpawner, roomCenter });
}

void SpawnRoom::generateDoors(RoomPlan& roomPlan, DoorDirection dir)
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

		// set door tiles
		for (int i = 0; i < door.spanTiles; i++) {
			roomPlan.setTile(row, colStart + 1, Tile::DOOR);
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

		// set door tiles
		for (int i = 0; i < door.spanTiles; i++) {
			roomPlan.setTile(rowStart + 1, col, Tile::DOOR);
		}
	}

	roomPlan.doors.push_back(door);
}
