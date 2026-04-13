#include "PortalRoom.h"

/**
 * @file PortalRoom.cpp
 * @brief Implements generation for the portal room plan.
 */

/**
 * @brief Generates the portal room plan with fixed dimensions and a portal spawner/trigger.
 * @param id Room identifier.
 * @param type Requested room type (currently ignored; portal is enforced).
 * @param seed Base dungeon seed.
 * @return Generated room plan.
 */
RoomPlan PortalRoom::generateRoomPlan(int id, RoomType type, int seed)
{
	RoomPlan room;
	// Meta data
	room.id = id;
	room.type = RoomType::PORTAL;
	room.seed = seed + room.id;
	room.width = 11;
	room.height = 11;

	// Tile map initialisation
	room.tileMap.assign(room.height * room.width, Tile::FLOOR);

	// Tile map construction
	// Outer walls
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
 * @brief Adds a portal spawner and portal trigger at the room center.
 * @param roomPlan Room plan to modify.
 * @param interiorArea Interior tile area (currently unused).
 */
void PortalRoom::generateSpawnPoints(RoomPlan& roomPlan, int interiorArea)
{
	// Portal Spawner
	sf::Vector2i roomCenter = sf::Vector2i(roomPlan.width, roomPlan.height) / 2;
	roomPlan.spawners.push_back({ SpawnerType::PortalSpawner, roomCenter });
	// Portal Trigger
	roomPlan.triggers.push_back({ TriggerType::PortalTrigger, roomCenter });
}
