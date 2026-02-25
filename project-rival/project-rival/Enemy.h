#pragma once
#include <SFML/graphics.hpp>

class Enemy
{
public:
	Enemy() {};
	~Enemy() {};
	void init();
	void update(double dt);
	void render();

private:
	sf::RectangleShape e_body;
	sf::Vector2f e_velocity{ 0.f, 0.f };

};

