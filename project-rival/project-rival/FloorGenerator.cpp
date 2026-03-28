#include "FloorGenerator.h"
#include <random>

FloorPlan FloorGenerator::generateFloorPlan(int floorId, int dungeonSeed, int roomCount, bool isBossFloor)
{
	FloorPlan plan;
	plan.dungeonSeed = dungeonSeed;
	plan.floorId = floorId;
	plan.floorSeed = dungeonSeed + floorId;
	plan.isFinalFloor = isBossFloor;
	
	std::mt19937 rng(plan.floorSeed); // setting the seed for the engine

	//std::uniform_int_distribution roomCountRange(3, 10); 	////uncomment when we're happy with generation
	//int roomCount = roomCountRange(rng);
	roomCount = std::clamp(roomCount, 3, 10);


	//___________________________ Nodes (rooms) ___________________________//
	plan.rooms.clear();
	plan.rooms.reserve(roomCount);

	std::uniform_int_distribution<> middleRoomRange(RoomType::COMBAT, RoomType::ROOM_COUNT - 1); // start from mini boss room when implemented
	for (int roomId = 0; roomId < roomCount; roomId++) {
		if (roomId == 0) {
			// First Room
			plan.rooms.push_back({ roomId, RoomType::SPAWN });
		}
		else if (roomId == roomCount - 1) {
			// Last Room
			if (isBossFloor) plan.rooms.push_back({ roomId, RoomType::PORTAL }); // change to boss when implemented
			else plan.rooms.push_back({ roomId, RoomType::PORTAL });
		}
		else {
			// Middle Room(s)
			RoomType randRoom = static_cast<RoomType>(middleRoomRange(rng));
			plan.rooms.push_back({ roomId, randRoom });
		}
	}

	//___________________________ Edges (connections) ___________________________//
	plan.edges.clear();
	auto addEdge = [&](int a, int b)	// adds an edge between rooms a and b if valid and doesn't already exist
		{
			if (a == b ||
				a < 0 || b < 0 ||
				a >= roomCount || b >= roomCount) return;	// invalid edge

			for (const auto& edge : plan.edges)
				if (edge.id_a == a && edge.id_b == b ||
					edge.id_a == b && edge.id_b == a) return;	// edge already exists

			int firstRoomId = 0;
			int lastRoomId = roomCount - 1;
			if (firstRoomId == a && lastRoomId == b ||
				firstRoomId == b && lastRoomId == a) return;	// edge directly connecting spawn and portal rooms is not allowed

			plan.edges.push_back({ a, b });
		};

	for (int i = 1; i < roomCount; i++)	
	{
		if (i != roomCount - 1)
		{
			std::uniform_int_distribution<> roomIndexRange(0, i - 1);	// ensures that the new room is connected to at least one existing room
			int randRoomIndex = roomIndexRange(rng);
			addEdge(i, randRoomIndex);
		}
		else
		{
			std::uniform_int_distribution<> roomIndexRange(1, i - 1);
			int randRoomIndex = roomIndexRange(rng);
			addEdge(i, randRoomIndex);
		}
	}

	plan.buildAdjacencyList();

	return plan;
}
