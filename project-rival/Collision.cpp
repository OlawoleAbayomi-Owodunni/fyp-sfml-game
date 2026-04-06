#include "Collision.h"

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
