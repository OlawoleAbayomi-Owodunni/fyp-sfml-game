#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Enemy
{
public:
	Enemy();
	~Enemy();
	void init();
	void update(double dt);
	void render(RenderWindow& window);

private:
	
	RectangleShape e_body;
	Vector2f e_velocity{ 0.f, 0.f };
	float e_moveSpeed{ 150.f };
};

