#include "CombatRoom.h"

RoomPlan CombatRoom::generateRoom(int id, RoomType type, int seed)
{
	RoomPlan room;
	// Meta data
	room.id = id;
	room.type = type;
	room.seed = seed;
		// -> note here that this is where the height and width will be determined based on the type
	//height 10-12; width 15-20
	room.height = rand() % 4 + 10;
	room.width = rand() % 6 + 15;

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
	
	// Pillars
	generateObstacles(room, interiorArea);

	// Spawners
	generateSpawnPoints(room, interiorArea);

	// yet to do: room geometry like doors, etc.

	return room;
}

void CombatRoom::generateObstacles(RoomPlan& room, int interiorArea)
{
	int maxPillars = interiorArea / (room.tileSize / 2);	// 1 pillar per half tile size of interior space, rounded down

	int pillarCount = rand() % (maxPillars + 1);

	for (int i = 0; i < pillarCount; i++)
	{
		int size = rand() % 2 + 1; // 1x1 or 2x2

		const int minRow = 1;
		const int maxRow = room.height - size - 1;
		const int minCol = 1;
		const int maxCol = room.width - size - 1;

		if (maxRow <= minRow || maxCol <= minCol)
			break;	// not enough space for pillars, skip

		bool placed = false;
		for (int attempts = 0; attempts < 50 && !placed; attempts++)
		{
			int randRow = rand() % (maxRow - minRow + 1) + minRow;
			int randCol = rand() % (maxCol - minCol + 1) + minCol;

			// Check if space is clear for the pillar
			bool canPlace = true;
			for (int x = 0; x < size && canPlace; x++) {
				for (int y = 0; y < size && canPlace; y++) {	// x & y are offsets from the top-left corner of the pillar (if size is 2, we need to check the tile and the one to the right/below)
					if (room.getTile(randRow + x, randCol + y) != Tile::FLOOR) {
						canPlace = false;
					}
				}
			}

			if (!canPlace)
				continue;	// space is not clear, try another position

			// Place the pillar
			for (int x = 0; x < size; x++) {
				for (int y = 0; y < size; y++) {
					room.setTile(randRow + x, randCol + y, Tile::WALL);
				}
			}

			placed = true;
		}
	}
}

void CombatRoom::generateSpawnPoints(RoomPlan& room, int interiorArea)
{
	// Enemy Spawner
	const int minEnemies = 1 + interiorArea / (room.tileSize * 2);
	int maxEnemies = 10 + interiorArea / (room.tileSize / 2);

	if (maxEnemies < minEnemies)
		maxEnemies = minEnemies;

	int totalEnemies = minEnemies + (rand() % ((maxEnemies - minEnemies) + 1));

	std::vector<sf::Vector2i> occupiedPositions;
	for (int i = 0; i < totalEnemies; i++)
	{
		sf::Vector2i randPos = { rand() % (room.width - 2) + 1, rand() % (room.height - 2) + 1 }; // random position within the room, excluding walls

		Tile tileAtPos = room.getTile(randPos.y, randPos.x);
		while (tileAtPos == Tile::WALL ||
			std::find(occupiedPositions.begin(), occupiedPositions.end(), randPos) != occupiedPositions.end())
		{
			randPos = { rand() % (room.width - 2) + 1, rand() % (room.height - 2) + 1 };
			tileAtPos = room.getTile(randPos.y, randPos.x);
		}

		room.spawners.push_back({ SpawnerType::EnemySpawner, randPos });
		occupiedPositions.push_back(randPos);
	}
}
