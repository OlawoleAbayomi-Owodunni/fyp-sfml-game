#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class player
{
public:
	player();
	~player();

	void init();
	void update(double dt);
	void render(RenderWindow& window);

private:
	// FUNCTIONS
	void reset();

	// VARIABLES
	RectangleShape p_body;
};

