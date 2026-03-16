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
	void handleMovement(double dt);
	void render(RenderWindow& window);

	const Vector2f getPosition() const { return p_body.getPosition(); }

private:
	// FUNCTIONS
	void reset();

	// VARIABLES
	RectangleShape p_body;

	Vector2f p_velocity{ 0.f, 0.f };
	float p_moveSpeed{ 200.f };

	bool p_isController;

	// reticle
	CircleShape p_reticle;
	Vector2f p_aimDir{ 0.f,0.f };
	float p_reticleDistance;
	Vector2f p_prevMousePos;
};

