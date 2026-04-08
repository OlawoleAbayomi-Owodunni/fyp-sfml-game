#pragma once
#include <vector>
#include <algorithm>
#include "RoomBlueprint.h"

struct FloorNode {
	int id;
	RoomType roomType;
};

struct FloorEdge {
	int id_a, id_b;
};

struct FloorPlan {
	int dungeonSeed;
	int floorId;
	int floorSeed;
	bool isFinalFloor;

	std::vector<FloorNode> rooms;
	std::vector<FloorEdge> edges;
	std::vector<std::vector<int>> roomAdjacenctyList;	// ID of connected rooms

	void buildAdjacencyList() {
		roomAdjacenctyList.clear();
		roomAdjacenctyList.resize(rooms.size());

		for (auto& edge : edges)
		{
			// if either room ID is out of bounds, skip this edge (invalid edge)
			if (edge.id_a < 0 || edge.id_b < 0
				|| edge.id_a >= roomAdjacenctyList.size()
				|| edge.id_b >= roomAdjacenctyList.size()) continue;	// skip invalid edges
			
			// add bidirectional connection
			roomAdjacenctyList[edge.id_a].push_back(edge.id_b);
			roomAdjacenctyList[edge.id_b].push_back(edge.id_a);
		}
	}
};