#pragma once
#include "RoomBlueprint.h"

class SpawnRoom : public IRoomGenerator
{
public:
	RoomPlan generateRoom(int id, RoomType type, int seed) override;
	void generateSpawnPoints(RoomPlan& roomPlan, int interiorArea) override;
	void generateDoors(RoomPlan& roomPlan, DoorDirection dir) override;
};

