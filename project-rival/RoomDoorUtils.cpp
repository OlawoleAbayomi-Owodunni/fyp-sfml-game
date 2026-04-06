#include "RoomDoorUtils.h"

void RoomDoorUtils::clearAllDoors(RoomPlan& roomPlan)
{
	for (auto& tile : roomPlan.tileMap)
	{
		if (tile == Tile::DOOR)
			tile = Tile::WALL;
	}

	roomPlan.doors.clear();

	// physically remove the trigger from the vector
	roomPlan.triggers.erase(std::remove_if(roomPlan.triggers.begin(), roomPlan.triggers.end(),
		[](const TriggerPlan& t) {return t.type == TriggerType::DoorTrigger; }),
		roomPlan.triggers.end());
}

DoorDirection RoomDoorUtils::opposite(DoorDirection dir)
{
	switch (dir)
	{
	case DoorDirection::NORTH: return DoorDirection::SOUTH;
	case DoorDirection::SOUTH: return DoorDirection::NORTH;
	case DoorDirection::EAST: return DoorDirection::WEST;
	case DoorDirection::WEST: return DoorDirection::EAST;
	}
}

void RoomDoorUtils::addDoor(RoomPlan& roomPlan, DoorDirection dir)
{
	// Avoid duplicates per direction
	for (auto& door : roomPlan.doors)
	{
		if (door.direction == dir)
			return;	// door already exists in this direction, do not add another
	}

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
