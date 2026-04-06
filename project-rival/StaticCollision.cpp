#include "StaticCollision.h"

StaticCollision::StaticCollision(const sf::FloatRect& bounds, CollisionLayer layer, uint32_t mask)
	: sc_bounds(bounds), sc_profile({ layer, mask })
{
	sc_profile.layer = layer;
	sc_profile.mask = mask;
}

sf::FloatRect StaticCollision::getCollisionBounds() const
{
	return sc_bounds;
}

CollisionProfile StaticCollision::getCollisionProfile() const
{
	return sc_profile;
}
