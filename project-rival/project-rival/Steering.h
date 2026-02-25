#pragma once
#include <SFML/Graphics.hpp>

struct SteerungOutput
{
	// "Acceleration" we want to apply this frame (units per second^2).
	// Example: (100, 0) means "speed up to the right".
	sf::Vector2f linear{ 0.f,0.f };
};

struct SteeringAgent {
	sf::Vector2f position{ 0.f,0.f };
	sf::Vector2f velocity{ 0.f,0.f };
	float maxSpeed{ 0.f };
	float maxAcceleration{ 0.f };
};

class ISteeringBehaviour
{
public:
	virtual ~ISteeringBehaviour() {};

	virtual sf::Vector2f calculateSteeringForce(const SteeringAgent& agent) = 0;
};