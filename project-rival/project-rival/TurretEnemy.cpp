#include "TurretEnemy.h"

TurretEnemy::TurretEnemy(const sf::Vector2f pos, int totalHealth)
	:Enemy(pos, totalHealth)
{
	initBody(sf::Vector2f(50.f, 50.f), sf::Color::Blue);
	init();
}

void TurretEnemy::init()
{
}

void TurretEnemy::update(double dt)
{
}

void TurretEnemy::setTarget(const Vector2f& target)
{
	e_target = target;
}

void TurretEnemy::hitWall(sf::Vector2f oldPos)
{
}
