#pragma once
#include <SFML/Graphics.hpp>
#include "Collision.h"

enum class PickupType
{
	HEALTH,
	AMMO,
	SINGLE_COIN,
	CHEST_COIN,

	PICKUP_COUNT
};

class Pickup : public ICollidable
{
public:
	Pickup(PickupType type, const sf::Vector2f& worldPos, float tileSize);

	void render(sf::RenderWindow& window) const;

	CollisionProfile getCollisionProfile() const override { return p_collisionProfile; }
	sf::FloatRect getCollisionBounds() const override { return p_shape.getGlobalBounds(); }

	bool shouldDestroy() const { return p_shouldDestroy; }
	void destroy() { p_shouldDestroy = true; }

	int getEffectAmount() const { return p_effectAmount; }

private:
	void initVisual(float tileSize);

	PickupType p_type;
	sf::RectangleShape p_shape;
	bool p_shouldDestroy{ false };
	int p_effectAmount{ 0 };

	CollisionProfile p_collisionProfile{ CollisionLayer::PICKUP_OBJECT_LAYER, CollisionLayer::PLAYER_LAYER };

};

