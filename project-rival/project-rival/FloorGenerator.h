#pragma once
#include "FloorBlueprint.h"

class FloorGenerator
{
public:
	FloorPlan generateFloorPlan(int floorId, int dungeonSeed, bool isBossFloor);
};

