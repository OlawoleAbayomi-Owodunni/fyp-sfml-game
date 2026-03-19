#include "Enemy.h"
using namespace sf;

Enemy::Enemy(const sf::Vector2f startPos) :
	e_startPos(startPos)
{
	e_collisionProfile.layer = CollisionLayer::ENEMY_LAYER;
	e_collisionProfile.mask = CollisionLayer::PLAYER_LAYER | CollisionLayer::PLAYER_BULLET_LAYER;
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

sf::FloatRect Enemy::getCollisionBounds() const
{
	return e_body.getGlobalBounds();
}

CollisionProfile Enemy::getCollisionProfile() const
{
	return e_collisionProfile;
}
