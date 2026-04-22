#pragma once
#include "RoomBlueprint.h"
#include "StaticCollision.h"
#include "SFML/Graphics.hpp"
#include "CustomSprite.h"

class RoomInstance
{
public:
	void buildFromPlan(const RoomPlan& plan, const sf::Vector2f& worldPos);
	
	const std::vector<StaticCollision>& getStaticRoomColliders();

	void render(sf::RenderWindow& window, bool texturedMode);

	void reset();

private:
	std::vector<StaticCollision> ri_staticRoomColliders;
	std::vector<sf::RectangleShape> ri_staticRoomShapes;
	std::vector<CustomSprite> ri_staticRoomSprites;
};

