#pragma once

/**
 * @file DungeonPlan.h
 * @brief Declares `DungeonPlan`, a lightweight state container for dungeon progression.
 */

/**
 * @brief Tracks progression through a dungeon run.
 */
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

   /**
	 * @brief Checks whether the current floor is the final floor in the run.
	 * @return True if @c currentFloorId is at or beyond @c floorCount-1.
	 */
	bool isFinalFloor() {
		if (currentFloorId >= floorCount - 1)
			return true;
		else
			return false;
	};

 /**
	 * @brief Checks whether the run can advance to the next floor.
	 * @return True if the dungeon is not complete and the current floor is not the final floor.
	 */
	bool canAdvance() {
		if(!isDungeonComplete && !isFinalFloor())
			return true;
		else
			return false;
	}

   /**
	 * @brief Advances the dungeon to the next floor, or completes the run if on the final floor.
	 */
	void advanceFloor() {
		if (canAdvance())
			currentFloorId++;
		else if (isFinalFloor())
			isDungeonComplete = true;
	}
};

