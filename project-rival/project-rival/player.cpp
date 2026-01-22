#include "player.h"

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
	Vector2f direction{ 0.f, 0.f };
	if (Keyboard::isKeyPressed(Keyboard::Key::W)) { direction.y -= 1.f; }
	if (Keyboard::isKeyPressed(Keyboard::Key::S)) { direction.y += 1.f; }
	if (Keyboard::isKeyPressed(Keyboard::Key::A)) { direction.x -= 1.f; }
	if (Keyboard::isKeyPressed(Keyboard::Key::D)) { direction.x += 1.f; }

	if (direction.x != 0.f || direction.y != 0.f)
		direction = direction.normalized();
	p_velocity = direction * p_moveSpeed;

	//Vector2f currentPosition = p_body.getPosition();
	//currentPosition += p_velocity * static_cast<float>(dt);
	//p_body.setPosition(currentPosition);
	p_body.move(p_velocity * static_cast<float>(dt));
}

void player::render(RenderWindow& window)
{
	window.draw(p_body);
}

void player::processGameEvents(const sf::Event& event)
{
	// Currently no player-specific event processing.
}


void player::reset()
{
	p_body.setPosition(Vector2f(50., 50.f));
}