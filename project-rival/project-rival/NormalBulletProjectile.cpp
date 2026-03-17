#include "NormalBulletProjectile.h"

NormalBulletProjectile::NormalBulletProjectile(sf::Vector2f spawnPoint, sf::Vector2f direction) :
	Projectile(spawnPoint), nbp_dir(direction)
{
	init();
}

void NormalBulletProjectile::init()
{
	if (nbp_dir != sf::Vector2f{ 0.f,0.f })
		nbp_dir = nbp_dir.normalized();
}

void NormalBulletProjectile::update(double dt)
{
	p_body.move(nbp_dir * nbp_speed * static_cast<float>(dt));
}
