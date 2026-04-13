#pragma once
#include <vector>
#include <algorithm>
#include "RoomBlueprint.h"

/**
 * @file FloorBlueprint.h
 * @brief Declares data structures used to represent a floor graph.
 */

/**
 * @brief Node in a floor graph, representing a room.
 */
struct FloorNode {
	int id;
	RoomType roomType;
};

/**
 * @brief Edge in a floor graph, connecting two rooms by id.
 */
struct FloorEdge {
	int id_a, id_b;
};

/**
 * @brief Floor layout metadata and connectivity.
 */
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