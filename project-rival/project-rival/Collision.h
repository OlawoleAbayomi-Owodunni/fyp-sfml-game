#pragma once
#include "Sfml/Graphics.hpp"

enum CollisionLayer : uint32_t
{
	NONE = 0,

	// Entity layers
	PLAYER_LAYER = 1 << 0,
	ENEMY_LAYER = 1 << 1,
	PLAYER_BULLET_LAYER = 1 << 2,
	ENEMY_BULLET_LAYER = 1 << 3,

	// Static Environment layers
	WALL_LAYER = 1 << 4,

	// World Environment layers

	ALL = 0xFFFFFFFF
};

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
