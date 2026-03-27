#pragma once
#include "RoomBlueprint.h"

class CombatRoom : public IRoomGenerator
{
public:
	RoomPlan generateRoomPlan(int id, RoomType type, int seed) override;
	RoomPlan startNewWave();
	RoomPlan roomCleared();

private:
	void generateObstacles(RoomPlan& room, int interiorArea);
	void generateSpawnPoints(RoomPlan& room, int interiorArea) override;

	RoomPlan currentRoomPlan;
};

