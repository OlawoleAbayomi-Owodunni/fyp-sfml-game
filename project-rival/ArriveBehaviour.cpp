#include "ArriveBehaviour.h"

namespace 
{
	float length(const sf::Vector2f& v) //magnitude (line length of the vector)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}
	Vector2f normalise(const Vector2f& v) //normalised vector for direction
	{
		const float len = length(v);
		if (len == 0.f) return Vector2f(0.f, 0.f);
		return v / len;
	}

	Vector2f clampMagnitude(const Vector2f& v, float maxMagnitude) //clamp the magnitude to a max value (so that the agent doesn't accelerate too much)
	{
		const float len = length(v);
		if (len == 0.f) return Vector2f(0.f, 0.f);
		if (len <= maxMagnitude)return v;

		return normalise(v) * maxMagnitude;
	}
}

void ArriveBehaviour::setTarget(const Vector2f& target)
{
	ab_target = target;
}

const Vector2f ArriveBehaviour::getSteering(const SteeringAgent& agent)
{
	// 1) Calculate Direction Vector from Agent to Target
	const Vector2 dirToTarget = normalise(ab_target - agent.position);

	// 2) Calculate Distance to Target
	const float disToTarget = length(ab_target - agent.position);

	// 3) Calculate target speed based on distance to target (slow down as we get closer, stop if we get too close)
	float targetSpeed;
	if (disToTarget > ab_slowRadius)	// 3.1) If outside slow radius, go max speed
		targetSpeed = agent.maxSpeed;
	
	else								// 3.2) If within slow radius, slow down (proportional to distance to target)
		targetSpeed = agent.maxSpeed * (disToTarget / ab_slowRadius);

	if (disToTarget <= ab_stopRadius)	// 3.3) If within stop radius, stop moving
		targetSpeed = 0.f;



	// 4) Calculate desired Velocity (towards target, with target speed)
	const Vector2f desiredVelocity = dirToTarget * targetSpeed;

	// 5) Calculate steering (desired velocity - current velocity)
	Vector2f steering = desiredVelocity - agent.velocity;

	// 6) Clamp steering to the agents max acceleration
	steering = clampMagnitude(steering, agent.maxAcceleration);
	
	return steering;
}
