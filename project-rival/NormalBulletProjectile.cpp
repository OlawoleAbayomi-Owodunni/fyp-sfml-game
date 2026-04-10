#include "NormalBulletProjectile.h"

NormalBulletProjectile::NormalBulletProjectile(sf::Vector2f spawnPoint, sf::Vector2f direction, float speed, int damage, bool isFromPlayer) :
	Projectile(spawnPoint, direction, speed, damage, isFromPlayer)
{
	init();
}

void NormalBulletProjectile::init()
{
	if (p_direction != sf::Vector2f{ 0.f,0.f })
		p_direction = p_direction.normalized();
	else
		destroy();
}

void NormalBulletProjectile::update(float dt)
{
	p_body.move(p_direction * p_speed * dt);
	onExpire(dt);
}
