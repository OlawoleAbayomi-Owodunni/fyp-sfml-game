#include "Enemy.h"
using namespace sf;

namespace
{
	float length(const sf::Vector2f& v) //magnitude (line length of the vector)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}
}


Enemy::Enemy(const Vector2f pos) : e_startPos(pos)
{
	init();
}

Enemy::~Enemy()
{
}

void Enemy::init()
{
	//--------- body setup ---------//
	e_body.setSize(sf::Vector2f(100.f, 100.f));
	Vector2f size = e_body.getSize();
	e_body.setFillColor(Color::Red);
	e_body.setOrigin(size / 2.f);
	e_body.setPosition(e_startPos);

	//--------- steering setup ---------//
	e_agent.position = e_body.getPosition();
	e_agent.velocity = Vector2f(0.f, 0.f);
	e_agent.maxSpeed = 200.f; //consider making this a parameter of the constructor if we want different types of enemies with different speeds.
	e_agent.maxAcceleration = 100.f; //consider making this a parameter of the constructor if we want different types of enemies with different accelerations.

	e_currBehaviour = &e_seek; //default behaviour is seek, but we can change this later with setBehaviour() if we want to test other behaviours.
}

void Enemy::update(double dt)
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

		float dtS = static_cast<float>(dt);

		// 1) Get the steering (acceleration) from the current behaviour.
		const Vector2f steering = e_currBehaviour->getSteering(e_agent);

		// 2) v = v + a * dt
		e_agent.velocity += steering * dtS;

		// 3) Clamp velocity to max speed.
		const float speed = length(e_agent.velocity);
		if (speed > e_agent.maxSpeed && speed > 0.f) {
			e_agent.velocity = (e_agent.velocity / speed) * e_agent.maxSpeed;
		}

		// 4) p = p + v * dt
		e_agent.position += e_agent.velocity * dtS;

		e_body.setPosition(e_agent.position);
	}
}

void Enemy::render(sf::RenderWindow& window)
{
	window.draw(e_body);
}



//////-------------------------- Other Member Functions --------------------------///////

void Enemy::setTarget(const Vector2f& target)
{
	e_target = target;

	e_seek.setTarget(target);
	e_arrive.setTarget(target);
}

void Enemy::setBehaviour(ISteeringBehaviour* behaviour)
{
	e_currBehaviour = behaviour;
}
