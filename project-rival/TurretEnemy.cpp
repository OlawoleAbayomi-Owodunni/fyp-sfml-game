#include "TurretEnemy.h"

/**
 * @file TurretEnemy.cpp
 * @brief Implements a stationary turret enemy that fires projectiles.
 */

/**
 * @brief Constructs a turret enemy and initializes weapon state.
 * @param pos Spawn position.
 * @param totalHealth Maximum health.
 */
TurretEnemy::TurretEnemy(const sf::Vector2f pos, int totalHealth)
	:Enemy(pos, totalHealth)
{
	initBody(sf::Vector2f(50.f, 50.f), sf::Color::Blue);
	configureSprite("ASSETS/SPRITES/Everything.png", {}, sf::Vector2i(0, 0));
	init();
}

/**
 * @brief Initializes firing cooldown state.
 */
void TurretEnemy::init()
{
	te_fireTimer = 0.f;
}

/**
 * @brief Updates the turret enemy without projectile output.
 * @param dt Delta time in seconds.
 */
void TurretEnemy::update(float dt)
{
}

/**
 * @brief Updates weapon aiming and fires projectiles into the shared projectile list.
 * @param dt Delta time in seconds.
 * @param gameProjectiles Output list used to spawn `Projectile` instances.
 */
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

/**
 * @brief Renders the turret body and its weapon.
 * @param window Render target.
 */
void TurretEnemy::render(sf::RenderWindow& window, bool texturedMode)
{
	Enemy::render(window, texturedMode);

	te_weapon.render(window, texturedMode);
}

/**
 * @brief Sets the current target for aiming.
 * @param target Target position (typically the player).
 */
void TurretEnemy::setTarget(const Vector2f& target)
{
	e_target = target;
}

/**
 * @brief Handles wall collision resolution (turrets are stationary).
 */
void TurretEnemy::hitWall()
{
}
