#pragma once
#include "Projectile.h"

class NormalBulletProjectile : public Projectile
{
public:
	NormalBulletProjectile(sf::Vector2f spawnPoint);
	~NormalBulletProjectile() = default;
	void init() override;
	void update(double dt) override;

private:
	sf::Vector2f b_target;
};

