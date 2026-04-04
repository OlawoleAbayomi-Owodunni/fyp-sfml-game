#pragma once
#include "RoomBlueprint.h"

class PortalRoom : public IRoomGenerator
{
public:
	RoomPlan generateRoomPlan(int id, RoomType type, int seed) override;

private:
	void generateSpawnPoints(RoomPlan& roomPlan, int interiorArea) override;
};

