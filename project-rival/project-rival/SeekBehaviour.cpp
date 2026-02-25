#include "SeekBehaviour.h"
#include <cmath>

using namespace trig_helper;

namespace trig_helper
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

void SeekBehaviour::setTarget(const Vector2f& target)
{
	sb_target = target;
}

const Vector2f SeekBehaviour::getSteering(const SteeringAgent& agent)
{
	// 1) Calculate Vector from Agent to Target
	const Vector2f toTarget = sb_target - agent.position;
	if (toTarget == Vector2f(0.f, 0.f))
		return Vector2f();

	// 2) Calculate desired Velocity (towards target, with max speed)
	const Vector2f desiredVelocity = normalise(toTarget) * agent.maxSpeed;

	// 3) Calculate steering (desired velocity - current velocity)
	Vector2f steering = desiredVelocity - agent.velocity;

	// 4) Clamp steering to the agents max acceleration
	steering = clampMagnitude(steering, agent.maxAcceleration);

	return steering;
}