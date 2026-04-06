#pragma once
#include "FloorBlueprint.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct FloorLayout {
	std::vector<sf::Vector2i> roomGridPositions;
};

class FloorLayoutGenerator {
public:
	FloorLayout generateFloorLayout(const FloorPlan& floorPlan);
};