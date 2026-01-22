#include "player.h"
#include "InputManager.h"

player::player()
{
	init();
}

player::~player()
{
}

void player::init()
{
	p_body.setSize(Vector2f(100.f, 100.f));
	Vector2f size = p_body.getSize();
	p_body.setFillColor(Color::Blue);
	p_body.setOrigin(size / 2.f);
	p_body.setPosition(Vector2f(25., 25.f));

	p_velocity = Vector2f(0.f, 0.f);
}

void player::update(double dt)
{
	//--------- movement logic ---------//
	Vector2f direction{ 0.f, 0.f };

	const Vector2f ls = InputManager::pad().leftStick();
	const bool isControllerActive = (ls.x != 0.f || ls.y != 0.f);

	if (isControllerActive) // Gamepad input
	{
		direction += ls.normalized();
	}
	else // Keyboard input
	{
		if (Keyboard::isKeyPressed(Keyboard::Key::W)) { direction.y -= 1.f; }
		if (Keyboard::isKeyPressed(Keyboard::Key::S)) { direction.y += 1.f; }
		if (Keyboard::isKeyPressed(Keyboard::Key::A)) { direction.x -= 1.f; }
		if (Keyboard::isKeyPressed(Keyboard::Key::D)) { direction.x += 1.f; }

		if (direction.x != 0.f || direction.y != 0.f)
			direction = direction.normalized();
	}

	p_velocity = direction * p_moveSpeed;
	p_body.move(p_velocity * static_cast<float>(dt));

	if (InputManager::pad().rightTrigger()) InputManager::pad().setRumble(0.2f, 0.8f);
	else InputManager::pad().setRumble(0.0f, 0.0f);
}

void player::render(RenderWindow& window)
{
	window.draw(p_body);
}

const Vector2f player::getPosition()
{
	return p_body.getPosition();
}


void player::reset()
{
	p_body.setPosition(Vector2f(50., 50.f));
}