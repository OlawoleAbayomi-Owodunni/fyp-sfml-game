#include "Enemy.h"
using namespace sf;

Enemy::Enemy(const sf::Vector2f startPos, int totalHealth) :
	e_startPos(startPos), e_maxHealth(totalHealth)
{
	e_health = e_maxHealth;
	e_isDead = false;

	e_collisionProfile.layer = CollisionLayer::ENEMY_LAYER;
	e_collisionProfile.mask = CollisionLayer::PLAYER_LAYER | CollisionLayer::PLAYER_BULLET_LAYER | CollisionLayer::WALL_LAYER;
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

void Enemy::reset()
{
	e_body.setPosition(e_startPos);
	e_health = e_maxHealth;
	e_isDead = false;
}

sf::Vector2f Enemy::getPosition() const
{
	return e_body.getPosition();
}

sf::FloatRect Enemy::getCollisionBounds() const
{
	return e_body.getGlobalBounds();
}

CollisionProfile Enemy::getCollisionProfile() const
{
	return e_collisionProfile;
}

void Enemy::takeDamage(int damage)
{
	e_health -= damage;
	if (e_health <= 0)
		onDeath();
}

void Enemy::onDeath()
{
	e_isDead = true;
}

bool Enemy::isDead() const
{
	return e_isDead;
}
