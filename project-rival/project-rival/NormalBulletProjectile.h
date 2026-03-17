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
	sf::Vector2f nbp_dir;
	float nbp_speed{ 500.f };
};

