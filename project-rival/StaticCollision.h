#pragma once
#include "Collision.h"

class StaticCollision : public ICollidable
{
public:
	StaticCollision() = default;
	StaticCollision(const sf::FloatRect& bounds, CollisionLayer layer, uint32_t mask);

	sf::FloatRect getCollisionBounds() const override;
	CollisionProfile getCollisionProfile() const override;

private:
	sf::FloatRect sc_bounds;
	CollisionProfile sc_profile;
};

