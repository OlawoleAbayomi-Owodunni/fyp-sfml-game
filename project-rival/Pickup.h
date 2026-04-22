#pragma once
#include <SFML/Graphics.hpp>
#include "Collision.h"
#include "CustomSprite.h"

/**
 * @file Pickup.h
 * @brief Declares pick-up items and their types.
 */

/**
 * @brief Types of pickups that can be spawned in the world.
 */
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

	void render(sf::RenderWindow& window, bool texturedMode) const;

	CollisionProfile getCollisionProfile() const override { return p_collisionProfile; }
	sf::FloatRect getCollisionBounds() const override { return p_shape.getGlobalBounds(); }
	int getEffectAmount() const { return p_effectAmount; }
	PickupType getType() const { return p_type; }

	bool shouldDestroy() const { return p_shouldDestroy; }
	void destroy() { p_shouldDestroy = true; }


private:
	void initVisual(float tileSize);

	PickupType p_type;
	sf::RectangleShape p_shape;
	CustomSprite p_sprite;
	bool p_shouldDestroy{ false };
	int p_effectAmount{ 0 };

	CollisionProfile p_collisionProfile{ CollisionLayer::PICKUP_OBJECT_LAYER, CollisionLayer::PLAYER_LAYER };

};

