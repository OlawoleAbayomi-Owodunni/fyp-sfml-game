#pragma once
#include "RoomBlueprint.h"
#include "StaticCollision.h"
#include "SFML/Graphics.hpp"

class RoomInstance
{
public:
	void buildFromPlan(const RoomPlan& plan, const sf::Vector2f& worldPos);
	
	const std::vector<StaticCollision>& getStaticCollisions();

	void render(sf::RenderWindow& window);

	void reset();

private:
	std::vector<StaticCollision> ri_staticColliders;
	std::vector<sf::RectangleShape> ri_staticShapes;
};

