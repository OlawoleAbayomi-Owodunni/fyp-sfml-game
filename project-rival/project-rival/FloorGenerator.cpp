#include "FloorGenerator.h"
#include <random>

FloorPlan FloorGenerator::generateFloorPlan(int floorId, int dungeonSeed, int roomCount, bool isBossFloor)
{
	FloorPlan plan;
	plan.dungeonSeed = dungeonSeed;
	plan.floorId = floorId;
	plan.floorSeed = dungeonSeed + floorId;
	plan.isFinalFloor = isBossFloor;

	roomCount = std::clamp(roomCount, 3, 10);

	std::mt19937 rng(plan.floorSeed); // setting the seed for the engine

	//___________________________ Nodes (rooms) ___________________________//
	plan.rooms.clear();
	plan.rooms.reserve(roomCount);

	// first to last room: spawn room, middle rooms, portal room
	int roomId = 0;
	plan.rooms.push_back({ roomId, RoomType::SPAWN });	// first room

	// middle rooms
	std::uniform_int_distribution<> middleRoomRange(RoomType::COMBAT, RoomType::ROOM_COUNT - 1);		// start from mini boss room when implemented
	for (roomId; roomId < roomCount; roomId++) {
		RoomType randRoom = static_cast<RoomType>(middleRoomRange(rng));
		plan.rooms.push_back({ roomId, randRoom });
	}

	// last room
	RoomType finalRoom;
	if (isBossFloor) finalRoom = RoomType::PORTAL;	// change to boss room when implemented
	else finalRoom = RoomType::PORTAL;

	plan.rooms.push_back({ roomId, finalRoom });	// last room

	//___________________________ Edges (connections) ___________________________//
	plan.edges.clear();
	auto addEdge = [&](int a, int b)	// adds an edge between rooms a and b if valid and doesn't already exist
		{
			if (a == b ||
				a < 0 || b>0 ||
				a >= roomCount || b >= roomCount) return;	// invalid edge

			for (const auto& edge : plan.edges)
				if (edge.id_a == a && edge.id_b == b ||
					edge.id_a == b && edge.id_b == a) return;	// edge already exists

			plan.edges.push_back({ a, b });
		};

	for (int i = 1; i < roomCount; i++)	
	{
		std::uniform_int_distribution<> roomIndexRange(0, i - 1);	// ensures that the new room is connected to at least one existing room
		int randRoomIndex = roomIndexRange(rng);
		addEdge(i, randRoomIndex);
	}

	plan.buildAdjacencyList();

	return plan;
}
