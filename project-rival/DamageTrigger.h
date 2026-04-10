#pragma once
#include <SFML/Graphics.hpp>
#include "Collision.h"

class DamageTrigger :public ICollidable
{
public:
	DamageTrigger(const sf::Vector2f& center, const sf::Vector2f& size, float lifetime, int damage, uint32_t targetMask);

	void update(float dt);
	void render(sf::RenderWindow& window);

	const bool shouldDestroy() const { return dt_shouldDestroy; }
	const int damage() const { return dt_damage; }

	sf::FloatRect getCollisionBounds() const override { return dt_bounds; }
	CollisionProfile getCollisionProfile() const override { return dt_collisionProfile; }

private:
	void initShape(const sf::Vector2f& center, const sf::Vector2f& size);

	sf::RectangleShape dt_shape;
	sf::FloatRect dt_bounds;

	CollisionProfile dt_collisionProfile;

	float dt_lifetime;
	bool dt_shouldDestroy;

	int dt_damage;
};

