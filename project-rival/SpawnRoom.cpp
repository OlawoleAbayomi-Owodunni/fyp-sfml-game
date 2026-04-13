#include "SpawnRoom.h"

/**
 * @file SpawnRoom.cpp
 * @brief Implements generation for the spawn room plan.
 */

/**
 * @brief Generates the spawn room plan with fixed dimensions and a player spawner.
 * @param id Room identifier.
 * @param type Requested room type (currently ignored; spawn is enforced).
 * @param seed Base dungeon seed.
 * @return Generated room plan.
 */
RoomPlan SpawnRoom::generateRoomPlan(int id, RoomType type, int seed)
{
	RoomPlan room;
	// Meta data
	room.id = id;
	room.type = RoomType::SPAWN;
	room.seed = seed + room.id;
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

	return room;
}

/**
 * @brief Adds a centered player spawn point to the room plan.
 * @param roomPlan Room plan to modify.
 * @param interiorArea Interior tile area (currently unused).
 */
void SpawnRoom::generateSpawnPoints(RoomPlan& roomPlan, int interiorArea)
{
	// Player Spawner
	sf::Vector2i roomCenter = sf::Vector2i(roomPlan.width, roomPlan.height) / 2;
	roomPlan.spawners.push_back({ SpawnerType::PlayerSpawner, roomCenter });
}
