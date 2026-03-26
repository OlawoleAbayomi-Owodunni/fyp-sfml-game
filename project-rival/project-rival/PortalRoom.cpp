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

	// Doors
	generateDoors(plan, DoorDirection::SOUTH);

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

void PortalRoom::generateDoors(RoomPlan& roomPlan, DoorDirection dir)
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
