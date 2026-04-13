#include <iostream>

#include "player.h"
#include "InputManager.h"
#include "PlayerWeapons.h"

/**
 * @file player.cpp
 * @brief Implements player movement, aiming, damage/upgrade handling, and weapon usage.
 */

namespace {
   /**
	 * @brief Returns the Euclidean length of a 2D vector.
	 * @param v Input vector.
	 * @return Vector magnitude.
	 */
	float length(const sf::Vector2f& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}
}

/**
 * @brief Constructs the player and initializes its state.
 */
Player::Player()
{
	startUp();
	init();
}

/**
 * @brief Destructor.
 */
Player::~Player()
{
}

/**
 * @brief Initializes base stats and resets the player.
 */
void Player::init()
{
	// Base stats
	p_maxHealth = 100;
	p_moveSpeed = 200.f;
	p_maxAmmo = 50;
	p_playerAmmo = p_maxAmmo;


	// Weapons
	p_currentWeaponID = 0;
	p_rumbleTimer = 0.f;

	reset();
}

/**
 * @brief Updates movement, aiming, and weapon handling.
 * @param dt Delta time in seconds.
 * @param mousePos Mouse position in world coordinates.
 * @param mousePixelPos Mouse position in pixel coordinates.
 * @param gameProjectiles Output list for spawned projectiles.
 * @param instantiableTriggers Output list for spawned damage triggers.
 */
void Player::update(float dt, const sf::Vector2f& mousePos, const sf::Vector2i& mousePixelPos,
	std::vector<std::unique_ptr<Projectile>>& gameProjectiles, 
	std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers)
{
	p_prevPos = p_body.getPosition();

	handleMovement(dt);
	handleAiming(mousePos, mousePixelPos);

	ManageWeapons(instantiableTriggers, gameProjectiles, dt);
}

/**
 * @brief Renders the player body, reticle, and currently equipped weapon.
 * @param window Render target.
 */
void Player::render(sf::RenderWindow& window)
{
	window.draw(p_body);
	window.draw(p_reticle);

	p_weapons[p_currentWeaponID]->render(window);
}


/**
 * @brief Applies damage to the player and marks it dead at zero health.
 * @param damage Damage amount.
 */
void Player::takeDamage(int damage)
{
	p_health -= damage;
	std::cout << "Health: " << p_health << "\n";
	if (p_health <= 0)
	{
		p_isDead = true;
	}
}

/**
 * @brief Reverts the player's position to the previous frame.
 */
void Player::hitWall()
{
	p_body.setPosition(p_prevPos);
}

/**
 * @brief Restores player health up to the max.
 * @param amount Health to add.
 */
void Player::heal(int amount)
{
	p_health += amount;
	if (p_health > p_maxHealth)
		p_health = p_maxHealth;
}

/**
 * @brief Adds ammo up to the max ammo capacity.
 * @param amount Ammo to add.
 */
void Player::addAmmo(int amount)
{
	p_playerAmmo += amount;
	if (p_playerAmmo > p_maxAmmo)
		p_playerAmmo = p_maxAmmo;
}

/**
 * @brief Applies upgrade levels and recalculates derived player stats.
 * @param healthLevel Health upgrade level.
 * @param speedLevel Movement speed upgrade level.
 * @param ammoLevel Ammo capacity upgrade level.
 */
void Player::applyUpgrade(int healthLevel, int speedLevel, int ammoLevel)
{
	p_maxHealth = 100 + (healthLevel-1) * 25;
	p_health = p_maxHealth;
	p_moveSpeed = 200.f + (speedLevel-1) * 20.f;
	p_maxAmmo = 50 + (ammoLevel-1) * 10;
	p_playerAmmo = p_maxAmmo;
}

/**
 * @brief Adds a weapon to the loadout when capacity allows.
 *
 * Rebuilds the instantiated weapons list when the loadout changes.
 *
 * @param type Weapon type.
 * @param level Weapon level.
 * @return True if the weapon was added.
 */
bool Player::addWeaponToLoadout(WeaponType type, int level)
{
	if (p_weaponLoadout.size() >= MAX_WEAPONS)
		return false;

	p_weaponLoadout.push_back({ type, level });
	buildWeaponsFromLoadout();
	return true;
}

/**
 * @brief Removes a weapon from the loadout if at least one remains equipped.
 * @param slotIndex Loadout index to remove.
 * @return True if the weapon was removed.
 */
bool Player::dropWeaponFromLoadout(int slotIndex)
{
	if (slotIndex < 0 || slotIndex >= p_weaponLoadout.size() || p_weaponLoadout.size() <= 1)
		return false;

	p_weaponLoadout.erase(p_weaponLoadout.begin() + slotIndex);

	if (p_currentWeaponID >= p_weaponLoadout.size())
		p_currentWeaponID = p_weaponLoadout.size() - 1;

	buildWeaponsFromLoadout();
	return true;
}

/**
 * @brief Replaces the currently equipped weapon and returns the dropped weapon.
 * @param newType New weapon type.
 * @param newLevel New weapon level.
 * @param weaponToDrop Output dropped weapon information.
 * @return True if the swap succeeded.
 */
bool Player::swapCurrentWeapon(WeaponType newType, int newLevel, WeaponInLoadout& weaponToDrop)
{
	if (p_weaponLoadout.empty() || p_currentWeaponID < 0 || p_currentWeaponID >= p_weaponLoadout.size())
		return false;

	weaponToDrop = p_weaponLoadout[p_currentWeaponID];
	p_weaponLoadout[p_currentWeaponID] = { newType, newLevel };

	buildWeaponsFromLoadout();
	return true;
}


/**
 * @brief Sets up render shapes, collision filtering, and default starting weapons.
 */
void Player::startUp()
{
	//body
	p_body.setSize(sf::Vector2f(50.f, 75.f));
	sf::Vector2f size = p_body.getSize();
	p_body.setOrigin(size / 2.f);
	p_body.setFillColor(sf::Color::Green);

	//physics
	p_velocity = sf::Vector2f(0.f, 0.f);

	//reticle
	p_reticle.setSize(sf::Vector2f(20.f, 20.f));
	p_reticle.setOrigin(p_reticle.getSize() / 2.f);
	p_reticle.setFillColor(sf::Color::Transparent);
	p_reticle.setOutlineColor(sf::Color::Yellow);
	p_reticle.setOutlineThickness(2.f);
	p_reticle.setPosition(p_body.getPosition() + p_reticleDistance * p_aimDir);
	p_reticleDistance = 150.f;

	//collision
	p_collisionProfile.layer = CollisionLayer::PLAYER_LAYER;
	p_collisionProfile.mask = CollisionLayer::ENEMY_LAYER | CollisionLayer::ENEMY_BULLET_LAYER | CollisionLayer::WALL_LAYER | CollisionLayer::DOOR_LAYER | CollisionLayer::PORTAL_TRIGGER_LAYER | CollisionLayer::DOOR_TRIGGER_LAYER | CollisionLayer::DAMAGE_TRIGGER_LAYER | CollisionLayer::PICKUP_OBJECT_LAYER | CollisionLayer::CHEST_TRIGGER_LAYER | CollisionLayer::NPC_LAYER;

	addWeaponToLoadout(WeaponType::PISTOL, 1);
	addWeaponToLoadout(WeaponType::KNIFE, 1);
}

/**
 * @brief Resets position, health, death state, and gamepad rumble.
 */
void Player::reset()
{
	p_body.setPosition(sf::Vector2f(1400, 500));
	p_health = p_maxHealth;
	p_isDead = false;
	p_rumbleTimer = 0.f;
	p_lowRumble = 0.f;
	p_highRumble = 0.f;
}

/**
 * @brief Handles player movement using controller left stick or WASD.
 * @param dt Delta time in seconds.
 */
void Player::handleMovement(float dt)
{
	//--------- movement logic ---------//
	sf::Vector2f direction{ 0.f, 0.f };

	const sf::Vector2f ls = InputManager::pad().leftStick();
	const bool isControllerActive = (ls.x != 0.f || ls.y != 0.f);

	if (isControllerActive) // Gamepad input
	{
		direction += ls.normalized();
	}
	else // Keyboard input
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { direction.y -= 1.f; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { direction.y += 1.f; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { direction.x -= 1.f; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { direction.x += 1.f; }

		if (direction.x != 0.f || direction.y != 0.f)
			direction = direction.normalized();
	}

	p_velocity = direction * p_moveSpeed;
	p_body.move(p_velocity * dt);
}

/**
 * @brief Updates aim direction and reticle position.
 *
 * When the controller right stick moves, controller aiming is used; otherwise
 * mouse movement enables mouse aiming.
 *
 * @param mouseWorldPos Mouse position in world coordinates.
 * @param mousePixelPos Mouse position in pixel coordinates.
 */
void Player::handleAiming(const sf::Vector2f& mouseWorldPos, const sf::Vector2i& mousePixelPos)
{
	//--------- aiming logic ---------//
	
	// Are we on Controller or Mouse
	const sf::Vector2f rs = InputManager::pad().rightStick();
	const bool isRSMoved = (rs.x != 0.f || rs.y != 0.f);

	const sf::Vector2f mouseDelta = mouseWorldPos - p_prevMousePos;
	const sf::Vector2i mousePixelDelta = mousePixelPos - p_prevMousePixelPos;
	const bool isMouseMoved = (mousePixelDelta.x != 0 || mousePixelDelta.y != 0);

	if (isRSMoved) p_isController = true;
	else if (isMouseMoved) p_isController = false;

	// Calculate and normalise aim direction
	sf::Vector2f aimVector;
	if (p_isController) aimVector = rs;
	else aimVector = mouseWorldPos - p_body.getPosition();

	if (aimVector != sf::Vector2f(0.f, 0.f))	// avoid crash out on controller since we can't normalise a zero vector
		p_aimDir = aimVector.normalized();
	else p_aimDir = sf::Vector2f(0.f, 0.f);

	p_reticle.setPosition(p_body.getPosition() + p_aimDir * p_reticleDistance);

	p_prevMousePos = mouseWorldPos;
	p_prevMousePixelPos = mousePixelPos;
}

/**
 * @brief Updates weapon state, handles weapon switching, and fires when input is active.
 * @param instantiableTriggers Output list for spawned damage triggers.
 * @param gameProjectiles Output list for spawned projectiles.
 * @param dt Delta time in seconds.
 */
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

	//--------- attacking logic ---------//
	bool isAttacking = false;
	if (InputManager::pad().rightTrigger() || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
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
			int ammoCost;
			if (WeaponType::SHOTGUN == p_weaponLoadout[p_currentWeaponID].type)
				ammoCost = 5; // Shotgun consumes 5 ammo per shot
			else
				ammoCost = 1; // Other guns consume 1 ammo per shot

			if (p_playerAmmo >= ammoCost)
			{
				auto const bulletsBefore = gameProjectiles.size();
				p_weapons[p_currentWeaponID]->fire(fireInfo, gameProjectiles);
				isAttacking = (gameProjectiles.size() > bulletsBefore);

				if (isAttacking)
					p_playerAmmo -= ammoCost;
			}
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



/**
 * @brief Rebuilds instantiated weapon objects from the current loadout.
 */
void Player::buildWeaponsFromLoadout()
{
	p_weapons.clear();
	for (const auto& entry : p_weaponLoadout)
	{
		auto weapon = createWeapon(entry.type, entry.level);
		p_weapons.push_back(std::move(weapon));
	}

	if (p_weapons.empty()) 
		p_currentWeaponID = 0;
	else if (p_currentWeaponID >= p_weapons.size()) 
		p_currentWeaponID = p_weapons.size() - 1;
}

/**
 * @brief Factory for creating a weapon instance for the given type/level.
 * @param type Weapon type.
 * @param level Weapon level.
 * @return Newly created weapon instance, or null on invalid type.
 */
std::unique_ptr<Weapon> Player::createWeapon(WeaponType type, int level)
{
	switch (type)
	{
	case WeaponType::PISTOL: return std::make_unique<PistolWeapon>(level);
	case WeaponType::ASSAULT_RIFLE:	return std::make_unique<ARWeapon>(level);
	case WeaponType::SHOTGUN: return std::make_unique<ShotgunWeapon>(level);
	case WeaponType::KNIFE:	return std::make_unique<KnifeWeapon>();
	case WeaponType::SWORD:	return std::make_unique<SwordWeapon>();
	case WeaponType::AXE: return std::make_unique<AxeWeapon>();
	default:
		std::cerr << "Invalid weapon type requested: " << type << "\n";
		return nullptr;
	}
}
