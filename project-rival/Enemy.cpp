#include "Enemy.h"
#include "player.h"

Enemy::Enemy()
{
	init();
}

Enemy::~Enemy()
{
}

void Enemy::init()
{
	e_body.setSize(Vector2f(100.f, 100.f));
	Vector2f size = e_body.getSize();
	e_body.setFillColor(Color::Red);
	e_body.setOrigin(size / 2.f);
	e_body.setPosition(Vector2f(400.f, 300.f));
}

void Enemy::update(double dt)
{
	// follow player
	Vector2f direction = player().getPosition() - e_body.getPosition();
	if (direction.x != 0.f || direction.y != 0.f)
		direction = direction.normalized();
	e_velocity = direction * e_moveSpeed;
	e_body.move(e_velocity * static_cast<float>(dt));
}

void Enemy::render(RenderWindow& window)
{
	window.draw(e_body);
}