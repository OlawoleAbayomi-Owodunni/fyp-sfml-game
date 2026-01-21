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
	p_body.setSize(Vector2f(50.f, 50.f));
	Vector2f size = p_body.getSize();
	p_body.setFillColor(Color::Blue);
	p_body.setOrigin(size / 2.f);
	p_body.setPosition(Vector2f(25., 25.f));
}

void player::update(double dt)
{
	return;
}

void player::render(RenderWindow& window)
{
	window.draw(p_body);
}




void player::reset()
{
	p_body.setPosition(Vector2f(25., 25.f));
}