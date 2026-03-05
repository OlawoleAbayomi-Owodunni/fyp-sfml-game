#pragma once
#include <SFML/graphics.hpp>
#include "SeekBehaviour.h"
#include "ArriveBehaviour.h"

class Enemy
{
public:
	Enemy(const sf::Vector2f pos);
	~Enemy();

	void init();
	void update(double dt);
	void render(sf::RenderWindow &window);

	void setTarget(const Vector2f& target);
	void setBehaviour(ISteeringBehaviour* behaviour);

private:
	sf::RectangleShape e_body;
	sf::Vector2f e_velocity{ 0.f, 0.f };

	sf::Vector2f e_startPos;

	SteeringAgent e_agent;
	SeekBehaviour e_seek{ Vector2f(0.f,0.f) };
	ArriveBehaviour e_arrive{ Vector2f(0.f,0.f), 0.f, 250.f };
	ISteeringBehaviour* e_currBehaviour{ nullptr };

	sf::Vector2f e_target{ 0.f, 0.f };
	float e_arriveEnterDist;
	float e_arriveExitDist;
};

