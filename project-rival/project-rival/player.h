#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <Xinput.h>

#include "Projectile.h"
#include "Collision.h"

using namespace sf;

class Player : public ICollidable
{
public:
	Player();
	~Player();

	void init();
	void update(double dt, const Vector2f& mousePos);
	void render(RenderWindow& window);

	const Vector2f getPosition() const;
	sf::FloatRect getCollisionBounds() const override;
	CollisionProfile getCollisionProfile() const override;

	void setSpawnPosition(const Vector2f& spawnPos);

	void handleMovement(double dt);
	void handleAiming(const Vector2f mousePos);

	void takeDamage(int damage);

	void hitWall(sf::Vector2f oldPos);

	// this functions will likely move to weapon class when made
	std::vector<std::unique_ptr<Projectile>>& getProjectiles();

private:
	// FUNCTIONS
	void reset();

	// VARIABLES
	RectangleShape p_body;

	Vector2f p_velocity{ 0.f, 0.f };
	float p_moveSpeed{ 200.f };

	bool p_isController;

	// reticle
	CircleShape p_reticle;
	Vector2f p_aimDir{ 0.f,0.f };
	float p_reticleDistance;
	Vector2f p_prevMousePos;

	// collsion
	CollisionProfile p_collisionProfile;

	// entity stats
	int p_maxHealth;
	int p_health;

	// projectiles --> test. this will move to weapon class when made
	std::vector<std::unique_ptr<Projectile>> p_projectiles;
};

