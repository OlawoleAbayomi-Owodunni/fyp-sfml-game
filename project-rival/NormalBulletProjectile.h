#pragma once
#include "Projectile.h"

class NormalBulletProjectile : public Projectile
{
public:
	NormalBulletProjectile(sf::Vector2f spawnPoint, sf::Vector2f direction, float speed, int damage, bool isFromPlayer);
	~NormalBulletProjectile() = default;

	void init() override;
    void update(float dt) override;

};

