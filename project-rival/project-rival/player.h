#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <Xinput.h>

using namespace sf;

class player
{
public:
	player();
	~player();

	void init();
	void update(double dt);
	void render(RenderWindow& window);

	void processGameEvents(const sf::Event& event);

private:
	// FUNCTIONS
	void reset();

	// VARIABLES
	RectangleShape p_body;

	Vector2f p_velocity{ 0.f, 0.f };
	float p_moveSpeed{ 200.f };
};

