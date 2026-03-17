#include "Projectile.h"

Projectile::Projectile(sf::Vector2f spawnPoint) : p_spawnPoint(spawnPoint)
{
	init_body();
}

void Projectile::render(sf::RenderWindow& window)
{
	window.draw(p_body);
}

void Projectile::init_body()
{
	p_body.setSize(sf::Vector2f(10.f, 10.f));
	p_body.setFillColor(sf::Color::White);
	p_body.setOrigin(p_body.getSize() / 2.f);
	p_body.setPosition(p_spawnPoint);
}
