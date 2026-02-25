#include "Enemy.h"
using namespace sf;


void Enemy::init()
{
	e_body.setSize(sf::Vector2f(100.f, 100.f));
	Vector2f size = e_body.getSize();
	e_body.setFillColor(Color::Red);
	e_body.setOrigin(size / 2.f);
	e_body.setPosition(e_startPos);
}

void Enemy::update(double dt)
{
}

void Enemy::render(sf::RenderWindow& window)
{
	window.draw(e_body);
}
