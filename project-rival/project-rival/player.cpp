#include "player.h"
#include "InputManager.h"
#include "NormalBulletProjectile.h"

namespace {
	float length(const Vector2f& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}
}

player::player()
{
	init();
}

player::~player()
{
}

void player::init()
{
	//body
	p_body.setSize(Vector2f(50.f, 75.f));
	Vector2f size = p_body.getSize();
	p_body.setFillColor(Color::Green);
	p_body.setOrigin(size / 2.f);
	p_body.setPosition(Vector2f(25., 25.f));

	//physics
	p_velocity = Vector2f(0.f, 0.f);

	//reticle
	p_reticle.setRadius(10.f);
	p_reticle.setOrigin(Vector2f(p_reticle.getRadius(), p_reticle.getRadius()));
	p_reticle.setFillColor(Color::Transparent);
	p_reticle.setOutlineColor(Color::Yellow);
	p_reticle.setOutlineThickness(2.f);
	p_reticle.setPosition(p_body.getPosition() + p_reticleDistance * p_aimDir);
	p_reticleDistance = 150.f;
}

void player::update(double dt, const Vector2f& mousePos)
{
	handleMovement(dt);
	handleAiming(mousePos);

	if (InputManager::pad().rightTrigger()) InputManager::pad().setRumble(0.2f, 0.8f);
	else InputManager::pad().setRumble(0.0f, 0.0f);

	for (auto& bullet : p_projectiles)
		bullet->update(dt);
}


void player::render(RenderWindow& window)
{
	window.draw(p_body);
	window.draw(p_reticle);

	for (const auto& bullet : p_projectiles)
		bullet->render(window);
}

void player::reset()
{
	p_body.setPosition(Vector2f(50., 50.f));
}

void player::handleMovement(double dt)
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
}

void player::handleAiming(const Vector2f mousePos)
{
	//--------- aiming logic ---------//
	
	// Are we on Controller or Mouse
	const Vector2f rs = InputManager::pad().rightStick();
	const bool isRSMoved = (rs.x != 0.f || rs.y != 0.f);

	const Vector2f mouseDelta = mousePos - p_prevMousePos;
	const bool isMouseMoved = (length(mouseDelta) > 0.5f);

	if (isRSMoved) p_isController = true;
	else if (isMouseMoved) p_isController = false;

	// Calculate and normalise aim direction
	Vector2f aimVector;
	if (p_isController) aimVector = rs;
	else aimVector = mousePos - p_body.getPosition();

	if (aimVector != Vector2f(0.f, 0.f))	// avoid crash out on controller since we can't normalise a zero vector
		p_aimDir = aimVector.normalized();
	else p_aimDir = Vector2f(0.f, 0.f);

	p_reticle.setPosition(p_body.getPosition() + p_aimDir * p_reticleDistance);

	p_prevMousePos = mousePos;

	//--------- shooting logic ---------// -> this will move to weapon class when made
	if (InputManager::pad().rightTrigger() || Mouse::isButtonPressed(Mouse::Button::Left))
	{
		std::unique_ptr<NormalBulletProjectile> newBullet = std::make_unique<NormalBulletProjectile>(p_body.getPosition(), p_aimDir);
		p_projectiles.push_back(std::move(newBullet));
	}
}
