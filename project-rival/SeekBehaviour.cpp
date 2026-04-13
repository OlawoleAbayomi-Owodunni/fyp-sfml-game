#include "SeekBehaviour.h"

/**
 * @file SeekBehaviour.cpp
 * @brief Implements steering behavior that accelerates an agent toward a target.
 */

namespace
{
 /**
	 * @brief Returns the Euclidean length of a 2D vector.
	 * @param v Input vector.
	 * @return Vector magnitude.
	 */
	float length(const sf::Vector2f& v) //magnitude (line length of the vector)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	/**
	 * @brief Returns a normalized direction vector.
	 * @param v Input vector.
	 * @return Unit vector or (0,0) when input is zero.
	 */
	Vector2f normalise(const Vector2f& v) //normalised vector for direction
	{
		const float len = length(v);
		if (len == 0.f) return Vector2f(0.f, 0.f);
		return v / len;
	}

 /**
	 * @brief Clamps the magnitude of a vector.
	 * @param v Input vector.
	 * @param maxMagnitude Maximum allowed magnitude.
	 * @return Vector scaled down to @p maxMagnitude if needed.
	 */
	Vector2f clampMagnitude(const Vector2f& v, float maxMagnitude) //clamp the magnitude to a max value (so that the agent doesn't accelerate too much)
	{
		const float len = length(v);
		if (len == 0.f) return Vector2f(0.f, 0.f);
		if (len <= maxMagnitude)return v;

		return normalise(v) * maxMagnitude;
	}
}


/**
 * @brief Sets the target position the agent should seek.
 * @param target Target position.
 */
void SeekBehaviour::setTarget(const Vector2f& target)
{
	sb_target = target;
}

/**
 * @brief Computes the steering acceleration for seeking movement.
 * @param agent Steering agent state.
 * @return Steering vector (acceleration).
 */
const Vector2f SeekBehaviour::getSteering(const SteeringAgent& agent)
{
	// 1) Calculate Direction Vector from Agent to Target
	const Vector2f dirToTarget = sb_target - agent.position;
	if (dirToTarget == Vector2f(0.f, 0.f))
		return Vector2f();

	// 2) Calculate desired Velocity (towards target, with max speed)
	const Vector2f desiredVelocity = normalise(dirToTarget) * agent.maxSpeed;

	// 3) Calculate steering (desired velocity - current velocity)
	Vector2f steering = desiredVelocity - agent.velocity;

	// 4) Clamp steering to the agents max acceleration
	steering = clampMagnitude(steering, agent.maxAcceleration);

	return steering;
}