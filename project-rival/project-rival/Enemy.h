#pragma once
#include <SFML/graphics.hpp>

class Enemy
{
public:
	Enemy(const sf::Vector2f pos) : e_startPos(pos) {};
	~Enemy() {};
	void init();
	void update(double dt);
	void render(sf::RenderWindow &window);

private:
	sf::RectangleShape e_body;
	sf::Vector2f e_velocity{ 0.f, 0.f };

	sf::Vector2f e_startPos;
};

