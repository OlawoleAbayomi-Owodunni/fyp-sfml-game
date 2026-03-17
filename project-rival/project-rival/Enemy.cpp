#include "Enemy.h"
using namespace sf;

Enemy::Enemy(const sf::Vector2f startPos) :
	e_startPos(startPos)
{
}

void Enemy::initBody(const sf::Vector2f& size, const sf::Color& colour)
{
	//--------- body setup ---------//
	e_body.setSize(size);
	e_body.setFillColor(colour);
	e_body.setOrigin(size / 2.f);
	e_body.setPosition(e_startPos);
}

void Enemy::render(sf::RenderWindow& window)
{
	window.draw(e_body);
}
