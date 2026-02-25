#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

struct SteerungOutput
{
	// "Acceleration" we want to apply this frame (units per second^2).
	// Example: (100, 0) means "speed up to the right".
	Vector2f linear{ 0.f,0.f };
};

struct SteeringAgent {
	Vector2f position{ 0.f,0.f };
	Vector2f velocity{ 0.f,0.f };
	float maxSpeed{ 0.f };
	float maxAcceleration{ 0.f };
};

class ISteeringBehaviour
{
public:
	virtual ~ISteeringBehaviour() {};

	virtual const Vector2f getSteering(const SteeringAgent& agent) = 0;
};