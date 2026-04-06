#pragma once

struct DungeonPlan
{
	int seed;
	int currentFloorId;
	int floorCount;
	bool isDungeonComplete;

	void start(int dungeonSeed)
	{
		seed = dungeonSeed;
		currentFloorId = 0;
		floorCount = 3;
		isDungeonComplete = false;
	}

	bool isFinalFloor() {
		if (currentFloorId >= floorCount - 1)
			return true;
		else
			return false;
	};

	bool canAdvance() {
		if(!isDungeonComplete && !isFinalFloor())
			return true;
		else
			return false;
	}

	void advanceFloor() {
		if (canAdvance())
			currentFloorId++;
		else if (isFinalFloor())
			isDungeonComplete = true;
	}
};

