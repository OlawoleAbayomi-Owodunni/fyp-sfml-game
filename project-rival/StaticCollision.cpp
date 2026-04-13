#include "StaticCollision.h"

/**
 * @file StaticCollision.cpp
 * @brief Implements a simple static collidable with fixed bounds and collision profile.
 */

/**
 * @brief Constructs a static collision object.
 * @param bounds World-space collision bounds.
 * @param layer Collision layer.
 * @param mask Collision mask of layers this object can collide with.
 */
StaticCollision::StaticCollision(const sf::FloatRect& bounds, CollisionLayer layer, uint32_t mask)
	: sc_bounds(bounds), sc_profile({ layer, mask })
{
	sc_profile.layer = layer;
	sc_profile.mask = mask;
}

/**
 * @brief Returns the static collision bounds.
 */
sf::FloatRect StaticCollision::getCollisionBounds() const
{
	return sc_bounds;
}

/**
 * @brief Returns the static collision profile.
 */
CollisionProfile StaticCollision::getCollisionProfile() const
{
	return sc_profile;
}
