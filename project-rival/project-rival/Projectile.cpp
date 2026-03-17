#include "Projectile.h"

Projectile::Projectile(sf::Vector2f spawnPoint, float lifetime) :
	p_spawnPoint(spawnPoint), p_lifetime(lifetime)
{
	init_body();
}

void Projectile::render(sf::RenderWindow& window)
{
	window.draw(p_body);
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

