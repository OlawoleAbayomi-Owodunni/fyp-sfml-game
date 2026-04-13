#pragma once
#include "Sfml/Graphics.hpp"

/**
 * @file Collision.h
 * @brief Declares collision layers, collision profiles, and the collidable interface.
 */

/**
 * @brief Bitmask layers used for collision filtering.
 */
enum CollisionLayer : uint32_t
{
	NONE = 0,

	// Entity layers
	PLAYER_LAYER = 1 << 0,
	ENEMY_LAYER = 1 << 1,
	PLAYER_BULLET_LAYER = 1 << 2,
	ENEMY_BULLET_LAYER = 1 << 3,
	NPC_LAYER = 1 << 4,

	// Static Environment layers
	WALL_LAYER = 1 << 5,
	DOOR_LAYER = 1 << 6,

	// World Environment layers

	// Trigger layers
	PORTAL_TRIGGER_LAYER = 1 << 7,
	DOOR_TRIGGER_LAYER = 1 << 8,
	DAMAGE_TRIGGER_LAYER = 1 << 9,
	CHEST_TRIGGER_LAYER = 1 << 10,

	// Game Object Layers
	PICKUP_OBJECT_LAYER = 1 << 11,

	ALL = 0xFFFFFFFF
};

/**
 * @brief Collision rules for an object (its layer and what layers it can collide with).
 */
struct CollisionProfile
{
	CollisionLayer layer;
	uint32_t mask;

	bool canCollideWith(const CollisionProfile& other) const
	{
		//  check if the other object's layer is in our mask AND also if our layer is in the other object's mask.
		return (((mask & other.layer) != CollisionLayer::NONE) && ((other.mask & layer) != CollisionLayer::NONE));
	}
};

class ICollidable
{
public:
	virtual ~ICollidable() = default;

	virtual sf::FloatRect getCollisionBounds() const = 0;
	virtual CollisionProfile getCollisionProfile() const = 0;
};

class CollisionCheck {
	public:
		static bool areColliding(const ICollidable& entity_A, const ICollidable& entity_B);
};
