#include "Collision.h"

/**
 * @file Collision.cpp
 * @brief Implements collision queries using collidable bounds and collision profiles.
 */

/**
 * @brief Tests whether two collidable entities should and do intersect.
 *
 * Collision is only evaluated when the collision profiles allow interaction.
 *
 * @param entity_A First entity.
 * @param entity_B Second entity.
 * @return True if profiles match and bounds overlap.
 */
bool CollisionCheck::areColliding(const ICollidable& entity_A, const ICollidable& entity_B)
{
	const CollisionProfile pa = entity_A.getCollisionProfile();
	const CollisionProfile pb = entity_B.getCollisionProfile();

	const sf::FloatRect cba = entity_A.getCollisionBounds();
	const sf::FloatRect cbb = entity_B.getCollisionBounds();


	if (pa.canCollideWith(pb))
	{
		bool isIntersecting = cba.findIntersection(cbb).has_value();
		return isIntersecting;
	}
	else
		return false;
}
