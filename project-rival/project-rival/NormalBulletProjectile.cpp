#include "NormalBulletProjectile.h"

NormalBulletProjectile::NormalBulletProjectile(sf::Vector2f spawnPoint, sf::Vector2f direction, bool isFromPlayer) :
	Projectile(spawnPoint, nbp_lifetime, isFromPlayer), nbp_dir(direction)
{
	init();
}

void NormalBulletProjectile::init()
{
	if (nbp_dir != sf::Vector2f{ 0.f,0.f })
		nbp_dir = nbp_dir.normalized();
	else
		destroy();

	// set damage
	p_damage = 1;
}

void NormalBulletProjectile::update(double dt)
{
	float dtf = static_cast<float>(dt);
	p_body.move(nbp_dir * nbp_speed * dtf);
	onExpire(dtf);
}
