#include "PortalRoom.h"

RoomPlan PortalRoom::generateRoom(int id, RoomType type, int seed)
{
	RoomPlan plan;
	// Meta data
	plan.id = id;
	plan.type = RoomType::PORTAL;
	plan.seed = seed;
	plan.width = 11;
	plan.height = 11;

	// Tile map initialisation
	plan.tileMap.assign(plan.height * plan.width, Tile::FLOOR);

	// Tile map construction
	// Outer walls
	for (int col = 0; col < plan.width; col++)
	{
		plan.setTile(0, col, Tile::WALL);				// top row
		plan.setTile(plan.height - 1, col, Tile::WALL);	// bottom row
	}
	for (int row = 0; row < plan.height; row++)
	{
		plan.setTile(row, 0, Tile::WALL);				// left column
		plan.setTile(row, plan.width - 1, Tile::WALL);	// right column
	}

	const int interiorWidth = plan.width - 2;
	const int interiorHeight = plan.height - 2;
	const int interiorArea = interiorWidth * interiorHeight;

	// Spawners
	generateSpawnPoints(plan, interiorArea);

	return plan;
}

void PortalRoom::generateSpawnPoints(RoomPlan& roomPlan, int interiorArea)
{
	// Portal Spawner
	sf::Vector2i roomCenter = sf::Vector2i(roomPlan.width, roomPlan.height) / 2;
	roomPlan.spawners.push_back({ SpawnerType::PortalSpawner, roomCenter });
	// Portal Trigger
	roomPlan.triggers.push_back({ TriggerType::PortalTrigger, roomCenter });
}
