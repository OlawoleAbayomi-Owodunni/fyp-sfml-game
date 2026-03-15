#pragma once
#include "Steering.h"

class SeekBehaviour : public ISteeringBehaviour
{
public:
	SeekBehaviour(const Vector2f& target) : sb_target(target) {};

	void setTarget(const Vector2f& target);

	const Vector2f getSteering(const SteeringAgent& agent) override;

private:
	Vector2f sb_target;
};

