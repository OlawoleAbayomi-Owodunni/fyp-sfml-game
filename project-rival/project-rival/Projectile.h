#pragma once
#include "SFML/Graphics.hpp"

#include "Collision.h"

class Projectile : public ICollidable
{
public:
	Projectile(sf::Vector2f spawnPoint, float lifetime, bool isFromPlayer);
	virtual ~Projectile() = default;

	virtual void init() = 0;
	virtual void update(double dt) = 0;
	virtual void render(sf::RenderWindow& window);

	virtual sf::FloatRect getCollisionBounds() const override;
	virtual CollisionProfile getCollisionProfile() const override;

	virtual bool shouldDestroy() const;

protected:
	void init_body();
	virtual void onExpire(float dt);
	virtual void destroy();

	bool p_shouldDestroy{ false };

	sf::RectangleShape p_body;
	sf::Vector2f p_spawnPoint;
	sf::Vector2f p_target;

	bool p_isFromPlayer;
	CollisionProfile p_collisionProfile;

	float p_lifetime{ 50.f };
};

