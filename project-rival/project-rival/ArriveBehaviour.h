#pragma once
#include "Steering.h"

class ArriveBehaviour : public ISteeringBehaviour
{
public:
	ArriveBehaviour(const Vector2f& target, float slowRadius, float stopRadius) :ab_target(target), ab_slowRadius(slowRadius), ab_stopRadius(stopRadius) {};

	void setTarget(const Vector2f& target);

	const Vector2f getSteering(const SteeringAgent& agent) override;

	const float getSlowRadius() const { return ab_slowRadius; }
	const float getStopRadius() const { return ab_stopRadius; }

private:
	Vector2f ab_target;
	float ab_slowRadius;
	float ab_stopRadius;
};

