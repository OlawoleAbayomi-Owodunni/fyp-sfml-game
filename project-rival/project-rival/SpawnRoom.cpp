#include "SpawnRoom.h"

RoomPlan SpawnRoom::generateRoom(int id, RoomType type, int seed)
{
	RoomPlan room;
	// Meta data
	room.id = id;
	room.type = type;
	room.seed = seed;
	room.height = 8;
	room.width = 8;

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
}

void SpawnRoom::generateSpawnPoints(RoomPlan& roomPlan, int interiorArea)
{
	// Player Spawner
	sf::Vector2i roomCenter = sf::Vector2i(roomPlan.width, roomPlan.height) / 2;
	roomPlan.spawners.push_back({ SpawnerType::PlayerSpawner, roomCenter });
}
