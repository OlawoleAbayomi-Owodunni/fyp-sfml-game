#include <iostream>

#include "player.h"
#include "InputManager.h"
#include "PlayerWeapons.h"

namespace {
	float length(const Vector2f& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}
}

Player::Player()
{
	init();
}

Player::~Player()
{
}

void Player::init()
{
	//body
	p_body.setSize(Vector2f(50.f, 75.f));
	Vector2f size = p_body.getSize();
	p_body.setFillColor(Color::Green);
	p_body.setOrigin(size / 2.f);

	//physics
	p_velocity = Vector2f(0.f, 0.f);

	//reticle
	p_reticle.setSize(Vector2f(20.f, 20.f));
	p_reticle.setOrigin(p_reticle.getSize() / 2.f);
	p_reticle.setFillColor(Color::Transparent);
	p_reticle.setOutlineColor(Color::Yellow);
	p_reticle.setOutlineThickness(2.f);
	p_reticle.setPosition(p_body.getPosition() + p_reticleDistance * p_aimDir);
	p_reticleDistance = 150.f;

	//collision
	p_collisionProfile.layer = CollisionLayer::PLAYER_LAYER;
	p_collisionProfile.mask = CollisionLayer::ENEMY_LAYER | CollisionLayer::ENEMY_BULLET_LAYER | CollisionLayer::WALL_LAYER | CollisionLayer::DOOR_LAYER | CollisionLayer::PORTAL_TRIGGER_LAYER | CollisionLayer::DOOR_TRIGGER_LAYER | CollisionLayer::DAMAGE_TRIGGER_LAYER;

	// stats
	p_maxHealth = 100;
	p_isDead = false;

	// Weapons
	p_weapons.clear();

	p_weapons.push_back(std::make_unique<PistolWeapon>());
	p_weapons.push_back(std::make_unique<ARWeapon>());
	p_weapons.push_back(std::make_unique<ShotgunWeapon>());

	p_weapons.push_back(std::make_unique<KnifeWeapon>());
	p_weapons.push_back(std::make_unique<SwordWeapon>());
	p_weapons.push_back(std::make_unique<AxeWeapon>());

	p_currentWeaponID = 0;
	
	p_rumbleTimer = 0.f;

	reset();
}

void Player::update(float dt, const Vector2f& mousePos, 
	std::vector<std::unique_ptr<Projectile>>& gameProjectiles, 
	std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers)
{
	p_prevPos = p_body.getPosition();

	handleMovement(dt);
	handleAiming(mousePos);

	ManageWeapons(instantiableTriggers, gameProjectiles, dt);
}

void Player::render(RenderWindow& window)
{
	window.draw(p_body);
	window.draw(p_reticle);

	p_weapons[p_currentWeaponID]->render(window);
}

#pragma region Getters and Setters
const Vector2f Player::getPosition() const
{
	return p_body.getPosition();
}

sf::FloatRect Player::getCollisionBounds() const
{
	return p_body.getGlobalBounds();
}

CollisionProfile Player::getCollisionProfile() const
{
	return p_collisionProfile;
}

void Player::setSpawnPosition(const Vector2f& spawnPos)
{
	p_body.setPosition(spawnPos);
}
#pragma endregion

void Player::reset()
{
	p_body.setPosition(Vector2f(1400, 500));
	p_health = p_maxHealth;
	p_isDead = false;
	p_rumbleTimer = 0.f;
	p_lowRumble = 0.f;
	p_highRumble = 0.f;
}

void Player::handleMovement(float dt)
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
	p_body.move(p_velocity * dt);
}

void Player::handleAiming(const Vector2f mousePos)
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

}

void Player::ManageWeapons(std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers, std::vector<std::unique_ptr<Projectile>>& gameProjectiles, float dt)
{
	// Update Weapon (for positioning and firing cooldowns)
	if (InputManager::pad().pressed(GamepadButton::RightBumper))
	{
		p_currentWeaponID++;
		if (p_currentWeaponID >= p_weapons.size())
			p_currentWeaponID = 0;
	}
	if (InputManager::pad().pressed(GamepadButton::LeftBumper))
	{
		p_currentWeaponID--;
		if (p_currentWeaponID < 0)
			p_currentWeaponID = p_weapons.size() - 1;
	}

	//--------- shooting logic ---------//
	bool isAttacking = false;
	if (InputManager::pad().rightTrigger() || Mouse::isButtonPressed(Mouse::Button::Left))
	{
		FireReq fireInfo;
		fireInfo.aimDir = p_aimDir;
		fireInfo.isFromPlayer = true;

		if (p_weapons[p_currentWeaponID]->isMelee())
		{
			auto const triggersBefore = instantiableTriggers.size();
			p_weapons[p_currentWeaponID]->fire(fireInfo, instantiableTriggers);
			isAttacking = (instantiableTriggers.size() > triggersBefore);
		}
		else
		{
			auto const bulletsBefore = gameProjectiles.size();
			p_weapons[p_currentWeaponID]->fire(fireInfo, gameProjectiles);
			isAttacking = (gameProjectiles.size() > bulletsBefore);
		}
	}

	// Rumble logic
	if (isAttacking)
	{
		switch (p_weapons[p_currentWeaponID]->rumbleIntensity())
		{
		case Intensity::LOW:
			p_rumbleTimer = 0.05f;
			p_lowRumble = 0.2f;
			p_highRumble = 0.2f;
			break;
		case Intensity::MEDIUM:
			p_rumbleTimer = 0.05f;
			p_lowRumble = 0.2f;
			p_highRumble = 0.5f;
			break;
		case Intensity::HIGH:
			p_rumbleTimer = 0.1f;
			p_lowRumble = 0.8f;
			p_highRumble = 0.8f;
			break;
		}
	}

	if (p_rumbleTimer > 0.f) {
		p_rumbleTimer -= dt;
		InputManager::pad().setRumble(p_lowRumble, p_highRumble);
	}
	else
		InputManager::pad().setRumble(0.0f, 0.0f);


	p_weapons[p_currentWeaponID]->update(dt, p_body.getPosition(), p_aimDir);
}


void Player::takeDamage(int damage)
{
	p_health -= damage;
	std::cout << "Health: " << p_health << "\n";
	if (p_health <= 0)
	{
		p_isDead = true;
	}
}

void Player::hitWall()
{
	p_body.setPosition(p_prevPos);
}

