#pragma once
#include<iostream>
#include <SFML/graphics.hpp>

#include "SeekBehaviour.h"
#include "ArriveBehaviour.h"

#include "Collision.h"

class Enemy : public ICollidable
{
public:
	Enemy(const sf::Vector2f startPos);

	virtual void init() = 0;
	virtual void update(double dt) = 0;
	virtual void render(sf::RenderWindow& window);

	virtual void setTarget(const Vector2f& target) = 0;

protected:
	void initBody(const sf::Vector2f& size, const sf::Color& colour);

	sf::RectangleShape e_body;
	sf::Vector2f e_startPos;
	sf::Vector2f e_target{ 0.f, 0.f };

	CollisionProfile e_collisionProfile;
};

