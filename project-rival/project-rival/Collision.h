#pragma once
#include "Sfml/Graphics.hpp"

enum CollisionLayer : uint32_t
{
	NONE = 0,

	PLAYER_LAYER = 1 << 0,
	ENEMY_LAYER = 1 << 1,
	PLAYER_BULLET_LAYER = 1 << 2,
	ENEMY_BULLET_LAYER = 1 << 3,
};


class ICollidable
{
public:
	struct CollisionProfile
	{
		CollisionLayer layer;
		CollisionLayer mask;

		bool canCollideWith(const CollisionProfile& other) const
		{
			//  check if the other object's layer is in our mask AND also if our layer is in the other object's mask.
			return (((mask & other.layer) != CollisionLayer::NONE) && ((other.mask & layer) != CollisionLayer::NONE));
		}
	};

	virtual ~ICollidable() = default;

	virtual sf::FloatRect getRectBounds() const = 0;
	virtual CollisionProfile getCollisionProfile() const = 0;
};

