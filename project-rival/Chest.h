#pragma once
#include <SFML/Graphics.hpp>
#include "Collision.h"
#include "CustomSprite.h"

class Chest : public ICollidable
{
public:
	Chest(const sf::Vector2f& worldPos, float tileSize);

	void render(sf::RenderWindow& window, bool texturedMode) const;

	sf::FloatRect getCollisionBounds() const override { return c_shape.getGlobalBounds(); }
	CollisionProfile getCollisionProfile() const override { return c_collisionProfile; }

	bool isOpened() const { return c_isOpened; }
	void open();

private:
	sf::RectangleShape c_shape;
	CustomSprite c_sprite;
	bool c_isOpened{ false };

	CollisionProfile c_collisionProfile{ CollisionLayer::CHEST_TRIGGER_LAYER, CollisionLayer::PLAYER_LAYER };
};

