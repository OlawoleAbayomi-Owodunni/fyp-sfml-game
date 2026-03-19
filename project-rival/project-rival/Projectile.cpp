#include "Projectile.h"

Projectile::Projectile(sf::Vector2f spawnPoint, float lifetime, bool isFromPlayer) :
	p_spawnPoint(spawnPoint), p_lifetime(lifetime), p_isFromPlayer(isFromPlayer)
{
	init_body();

	if(isFromPlayer)
	{
		p_collisionProfile.layer = CollisionLayer::PLAYER_BULLET_LAYER;
		p_collisionProfile.mask = CollisionLayer::ENEMY_LAYER;
	}
	else
	{
		p_collisionProfile.layer = CollisionLayer::ENEMY_BULLET_LAYER;
		p_collisionProfile.mask = CollisionLayer::PLAYER_LAYER;
	}
	// add other collision masks for environment (e.g. walls)
	// like so ----> p_collisionProfile.mask |= CollisionLayer::LEVEL_LAYER;
}

void Projectile::render(sf::RenderWindow& window)
{
	window.draw(p_body);
}

sf::FloatRect Projectile::getCollisionBounds() const
{
	return p_body.getGlobalBounds();
}

CollisionProfile Projectile::getCollisionProfile() const
{
	return p_collisionProfile;
}

bool Projectile::shouldDestroy() const
{
	return p_shouldDestroy;
}

void Projectile::init_body()
{
	p_body.setSize(sf::Vector2f(10.f, 10.f));
	p_body.setFillColor(sf::Color::White);
	p_body.setOrigin(p_body.getSize() / 2.f);
	p_body.setPosition(p_spawnPoint);
}

void Projectile::onExpire(float dt)
{
	p_lifetime -= dt;
	if (p_lifetime <= 0.f)
		destroy();
}

void Projectile::destroy()
{
	p_shouldDestroy = true;
}

