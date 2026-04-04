#pragma once
#include "RoomBlueprint.h"

class RoomDoorUtils
{
public:
	static void clearAllDoors(RoomPlan& roomPlan);

	static DoorDirection opposite(DoorDirection dir);

	static void addDoor(RoomPlan& roomPlan, DoorDirection dir);
};

