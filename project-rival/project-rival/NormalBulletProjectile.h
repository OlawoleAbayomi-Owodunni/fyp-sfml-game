#pragma once
#include "Projectile.h"

class NormalBulletProjectile : public Projectile
{
public:
	NormalBulletProjectile(sf::Vector2f spawnPoint, sf::Vector2f direction);
	~NormalBulletProjectile() = default;

	void init() override;
	void update(double dt) override;

private:
	static constexpr float nbp_lifetime = 5.f; // need this to be initialised at the samee time as projectile
	sf::Vector2f nbp_dir;
	float nbp_speed{ 500.f };
};

