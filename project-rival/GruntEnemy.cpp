#include "GruntEnemy.h"

/**
 * @file GruntEnemy.cpp
 * @brief Implements the melee/trigger-based enemy that uses steering behaviours.
 */

namespace
{
 /**
	 * @brief Computes the Euclidean length of a 2D vector.
	 * @param v Vector.
	 * @return Magnitude of the vector.
	 */
	float length(const sf::Vector2f& v) //magnitude (line length of the vector)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}
}

/**
 * @brief Constructs a grunt enemy and initializes steering and weapon state.
 * @param pos Spawn position.
 * @param totalHealth Maximum health.
 */
GruntEnemy::GruntEnemy(const sf::Vector2f pos, int totalHealth)
	:Enemy(pos, totalHealth)
{
	initBody(sf::Vector2f(50.f, 50.f), sf::Color::Red);
	configureSprite("ASSETS/SPRITES/Everything.png", {}, sf::Vector2i(0, 0));
	init();
}

/**
 * @brief Initializes steering configuration and weapon cooldown state.
 */
void GruntEnemy::init()
{
	//--------- steering setup ---------//
	e_agent.position = e_body.getPosition();
	e_agent.velocity = Vector2f(0.f, 0.f);
	e_agent.maxSpeed = 200.f; //consider making this a parameter of the constructor if we want different types of enemies with different speeds.
	e_agent.maxAcceleration = 100.f; //consider making this a parameter of the constructor if we want different types of enemies with different accelerations.

	e_currBehaviour = &e_seek; //default behaviour is seek, but we can change this later with setBehaviour() if we want to test other behaviours.

	//--------- behaviour setup ---------//
	// Derive switching distances from a "gap" value to avoid jittering when switching behaviours
	const float base = std::max(e_arrive.getSlowRadius(), e_arrive.getStopRadius());
	const float gap = 100.f;
	e_arriveEnterDist = base;
	e_arriveExitDist = base + gap;

	//--------- weapon setup ---------//
	ge_attackTimer = 0.f;
}

/**
 * @brief Updates the grunt enemy without external trigger output.
 * @param dt Delta time in seconds.
 */
void GruntEnemy::update(float dt)
{
}

/**
 * @brief Updates movement steering and conditionally fires melee/trigger attacks.
 *
 * Switches between seek/arrive behaviours based on distance thresholds to reduce
 * behaviour jitter.
 *
 * @param dt Delta time in seconds.
 * @param instantiableTriggers Output list used to spawn `DamageTrigger` instances.
 */
void GruntEnemy::update(float dt, std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers)
{
	if (e_currBehaviour != nullptr)
	{
		const float disToTarget = length(e_target - e_agent.position);

		// Auto Switchin behaviours
		if (e_currBehaviour == &e_seek) {
			if (disToTarget <= e_arriveEnterDist)
			{
				e_currBehaviour = &e_arrive;
			}
		}
		else if (e_currBehaviour == &e_arrive) {
			if (disToTarget > e_arriveExitDist)
			{
				e_currBehaviour = &e_seek;
			}
		}

		// 1) Get the steering (acceleration) from the current behaviour.
		const Vector2f steering = e_currBehaviour->getSteering(e_agent);

		// 2) v = v + a * dt
		e_agent.velocity += steering * dt;

		// 3) Clamp velocity to max speed.
		const float speed = length(e_agent.velocity);
		if (speed > e_agent.maxSpeed && speed > 0.f) {
			e_agent.velocity = (e_agent.velocity / speed) * e_agent.maxSpeed;
		}

		// 4) p = p + v * dt
		e_agent.position += e_agent.velocity * dt;

		e_body.setPosition(e_agent.position);

		//--------- attack logic ---------//
		if (e_currBehaviour == &e_arrive)
		{
			sf::Vector2f aimDir = e_target - e_body.getPosition();
			ge_weapon.update(dt, e_body.getPosition(), aimDir);

			ge_attackTimer -= dt;
			if (ge_attackTimer <= 0.f)
			{
				ge_attackTimer = static_cast<float>(rand() % 50) / 10.f;

				FireReq req;
				req.aimDir = aimDir;
				req.isFromPlayer = false;
				ge_weapon.fire(req, instantiableTriggers);
			}
		}
	}

}

/**
 * @brief Updates steering behaviour targets and records previous position for wall resolution.
 * @param target Target position (typically the player).
 */
void GruntEnemy::setTarget(const Vector2f& target)
{
	e_prevPos = e_body.getPosition();

	e_target = target;

	e_seek.setTarget(target);
	e_arrive.setTarget(target);
}

/**
 * @brief Reverts the enemy back to its previous position after a wall collision.
 */
void GruntEnemy::hitWall()
{
	e_agent.position = e_prevPos;
	e_body.setPosition(e_agent.position);
}

