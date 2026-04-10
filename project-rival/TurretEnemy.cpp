#include "TurretEnemy.h"

TurretEnemy::TurretEnemy(const sf::Vector2f pos, int totalHealth)
	:Enemy(pos, totalHealth)
{
	initBody(sf::Vector2f(50.f, 50.f), sf::Color::Blue);
	init();
}

void TurretEnemy::init()
{
	te_fireTimer = 0.f;
}

void TurretEnemy::update(float dt)
{
}

void TurretEnemy::update(float dt, std::vector<std::unique_ptr<Projectile>>& gameProjectiles)
{
	//Aim at player
	sf::Vector2f aimDir = e_target - e_body.getPosition();

	// Update weapons transform
	te_weapon.update(dt, e_body.getPosition(), aimDir);

	// Fire weapon if able
	te_fireTimer -= dt;
	if (te_fireTimer <= 0.f)
	{
		te_fireTimer = static_cast<float>(rand() % 400 + 100) / 100.f;
		
        FireReq req;
		req.aimDir = aimDir;
		req.isFromPlayer = false;
		te_weapon.fire(req, gameProjectiles);
	}
}

void TurretEnemy::render(sf::RenderWindow& window)
{
	Enemy::render(window);

	te_weapon.render(window);
}

void TurretEnemy::setTarget(const Vector2f& target)
{
	e_target = target;
}

void TurretEnemy::hitWall()
{
}
