#pragma once
#include "RoomBlueprint.h"

class CombatRoom : public IRoomGenerator
{
public:
	RoomPlan generateRoom(int id, RoomType type, int seed) override;
	void generateObstacles(RoomPlan& room, int interiorArea);
	void generateSpawnPoints(RoomPlan& room, int interiorArea) override;

};

