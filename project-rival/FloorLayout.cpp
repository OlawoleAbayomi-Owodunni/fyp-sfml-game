#include "FloorLayout.h"
#include <queue>

FloorLayout FloorLayoutGenerator::generateFloorLayout(const FloorPlan& floorPlan)
{
	FloorLayout layout;

	const int roomCount = floorPlan.rooms.size();
	layout.roomGridPositions.assign(roomCount, sf::Vector2i(0, 0));	// initialize all room positions to (0, 0)

	if (roomCount == 0) return layout;

	std::vector<bool> roomPlaced(roomCount, false);

	auto isOccupied = [&](const sf::Vector2i& pos)	// checks if the given position is already occupied by a placed room
		{
			for (int i = 0; i < roomCount; i++)
			{
				if (!roomPlaced[i]) continue;	// skip unplaced rooms

				if (layout.roomGridPositions[i] == pos)	// position is occupied by room i
					return true;

			}
			return false;
		};

	auto placeRoom = [&](int roomId, const sf::Vector2i& pos)
		{
			layout.roomGridPositions[roomId] = pos;
			roomPlaced[roomId] = true;
		};

	const sf::Vector2i directions[4] =
	{
		{0, -1}, //North
		{1, 0},  //East
		{0, 1},  //South
		{-1, 0}  //West
	};

	// Place the first room at the origin
	placeRoom(0, sf::Vector2i(0, 0));

	// Place the remaining rooms
	std::queue<int> queue;	// queue of rooms to place, initialized with the first room
	queue.push(0);

	while (!queue.empty())
	{
		int currentRoomId = queue.front();
		queue.pop();

		sf::Vector2i currentPos = layout.roomGridPositions[currentRoomId];

		for (int neighbour : floorPlan.roomAdjacenctyList[currentRoomId])
		{
			if (neighbour < 0 || neighbour >= roomCount) continue;	// skip invalid neighbour IDs
			if (roomPlaced[neighbour]) continue;	// skip already placed rooms

			const int startDir = (floorPlan.floorSeed + currentRoomId + neighbour) % 4;	// starting direction for placement, determined by floor seed and room IDs to add some variability

			bool placedNeighbour = false;

			// 1) Try to place the neighbour in one of the 4 directions around the current room, starting from a direction determined by the floor seed and room IDs to add some variability to the layout
			for (int tryDir = 0; tryDir < 4; tryDir++)
			{
				const int d = (startDir + tryDir) % 4;	// current direction to try
				const sf::Vector2i newPos = currentPos + directions[d];

				if (!isOccupied(newPos))
				{
					placeRoom(neighbour, newPos);
					placedNeighbour = true;
					break;
				}
			}

			// 2) If all 4 adjacent positions are occupied, place the neighbour in the first available position in an expanding square around the current room
			if (!placedNeighbour)
			{
				int dir = startDir;
				for (int step = 2; step <= 50; step++)
				{
					const sf::Vector2i newPos = currentPos + directions[dir] * step;
					if (!isOccupied(newPos))
					{
						placeRoom(neighbour, newPos);
						placedNeighbour = true;
						break;
					}
				}
			}

			// continue BFS either way
			queue.push(neighbour);
		}
	}

	return layout;
}
