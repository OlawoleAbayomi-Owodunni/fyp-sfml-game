#pragma once
#include "FloorBlueprint.h"
#include <SFML/Graphics.hpp>
#include <vector>

/**
 * @file FloorLayout.h
 * @brief Declares data used to position rooms of a generated floor in world/grid space.
 */

/**
 * @brief Grid positions for each room id in a floor.
 */
struct FloorLayout {
	std::vector<sf::Vector2i> roomGridPositions;
};

class FloorLayoutGenerator {
public:
	FloorLayout generateFloorLayout(const FloorPlan& floorPlan);
};