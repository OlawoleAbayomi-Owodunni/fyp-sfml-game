#include "TurretEnemy.h"

TurretEnemy::TurretEnemy(const sf::Vector2f pos, int totalHealth)
	:Enemy(pos, totalHealth)
{
	initBody(sf::Vector2f(50.f, 50.f), sf::Color::Blue);
	init();
}

void TurretEnemy::init()
{
	e_fireTimer = 0.f;
}

void TurretEnemy::update(float dt)
{
}

void TurretEnemy::update(float dt, std::vector<std::unique_ptr<Projectile>>& gameProjectiles)
{
	//Aim at player
	sf::Vector2f aimDir = e_target - e_body.getPosition();

	// Update weapons transform
	e_weapon.update(dt, e_body.getPosition(), aimDir);

	// Fire weapon if able
	e_fireTimer -= dt;
	if (e_fireTimer <= 0.f)
	{
		e_fireTimer = 1.f;
		
        FireReq req;
		req.aimDir = aimDir;
		req.isFromPlayer = false;
		e_weapon.fire(req, gameProjectiles);
	}
}

void TurretEnemy::render(sf::RenderWindow& window)
{
	Enemy::render(window);

	e_weapon.render(window);
}

void TurretEnemy::setTarget(const Vector2f& target)
{
	e_target = target;
}

void TurretEnemy::hitWall()
{
}
