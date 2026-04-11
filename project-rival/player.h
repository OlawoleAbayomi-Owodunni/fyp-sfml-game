#pragma once
#include <SFML/Graphics.hpp>

#include "Collision.h"
#include "Weapon.h"

using namespace sf;

class Player : public ICollidable
{
public:
	Player();
	~Player();

	void init();
	void update(float dt, const Vector2f& mousePos, std::vector<std::unique_ptr<Projectile>>& gameProjectiles, std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers);
	void ManageWeapons(std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers, std::vector<std::unique_ptr<Projectile>>& gameProjectiles, float dt);
	void render(RenderWindow& window);

	const Vector2f getPosition() const;
	sf::FloatRect getCollisionBounds() const override;
	CollisionProfile getCollisionProfile() const override;

	void setSpawnPosition(const Vector2f& spawnPos);
	void setBodyColor(const Color& color);

	void hitWall();

	void takeDamage(int damage);
	const bool isDead() const { return p_isDead; }

private:
	// FUNCTIONS
	void reset();
	void handleMovement(float dt);
	void handleAiming(const Vector2f mousePos);

	// VARIABLES
	RectangleShape p_body;

	Vector2f p_velocity{ 0.f, 0.f };
	float p_moveSpeed{ 200.f };

	Vector2f p_prevPos;

	bool p_isController;

	// reticle
	RectangleShape p_reticle;
	Vector2f p_aimDir{ 0.f,0.f };
	float p_reticleDistance;
	Vector2f p_prevMousePos;

	// collsion
	CollisionProfile p_collisionProfile;

	// entity stats
	int p_maxHealth;
	int p_health;
	bool p_isDead;

	// weapons
	std::vector<std::unique_ptr<Weapon>> p_weapons;
	std::unique_ptr<Weapon> p_currentWeapon;
	int p_currentWeaponID;
	
	float p_rumbleTimer;
	float p_lowRumble;
	float p_highRumble;

};
