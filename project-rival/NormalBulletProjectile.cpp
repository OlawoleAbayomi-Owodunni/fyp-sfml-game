#include "NormalBulletProjectile.h"

/**
 * @file NormalBulletProjectile.cpp
 * @brief Implements a simple straight-moving bullet projectile.
 */

/**
 * @brief Constructs a normal bullet projectile.
 * @param spawnPoint Initial world position.
 * @param direction Direction of travel.
 * @param speed Units per second.
 * @param damage Damage dealt on hit.
 * @param isFromPlayer True when spawned by the player.
 */
NormalBulletProjectile::NormalBulletProjectile(sf::Vector2f spawnPoint, sf::Vector2f direction, float speed, int damage, bool isFromPlayer) :
	Projectile(spawnPoint, direction, speed, damage, isFromPlayer)
{
	init();
}

/**
 * @brief Normalizes direction or destroys the projectile if direction is zero.
 */
void NormalBulletProjectile::init()
{
	if (p_direction != sf::Vector2f{ 0.f,0.f })
		p_direction = p_direction.normalized();
	else
		destroy();
}

/**
 * @brief Moves the bullet forward and applies lifetime expiry.
 * @param dt Delta time in seconds.
 */
void NormalBulletProjectile::update(float dt)
{
	p_body.move(p_direction * p_speed * dt);
	p_sprite.update(dt);
	onExpire(dt);
}
