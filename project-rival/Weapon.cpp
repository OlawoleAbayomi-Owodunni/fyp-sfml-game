#include "Weapon.h"
#include <cmath>

/**
 * @file Weapon.cpp
 * @brief Implements the shared weapon base used by player and enemy weapons.
 */

/**
 * @brief Constructs a weapon and initializes its stats and appearance.
 * @param type Weapon type.
 * @param level Upgrade/weapon level used to scale stats.
 */
Weapon::Weapon(WeaponType type, int level) : w_weaponLevel(level)
{
	init(type);
}

/**
 * @brief Initializes weapon visuals and combat parameters for a type.
 * @param type Weapon type.
 */
void Weapon::init(WeaponType type)
{
	const int level = std::clamp(w_weaponLevel, 1, 10);
	WeaponType weaponType = type;
	switch(weaponType)
	{
		// Guns
		case PISTOL:
			w_shape.setSize(sf::Vector2f(80.f, 20.f));
			w_shape.setFillColor(sf::Color::Blue);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.55f - (0.05f * (level - 1));
			w_rumbleIntensity = Intensity::LOW;
			w_damage = 10 + ((level - 1) * 4);
			break;

		case ASSAULT_RIFLE:
			w_shape.setSize(sf::Vector2f(120.f, 20.f));
			w_shape.setFillColor(sf::Color::Red);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.25f - (0.02f * (level - 1));
			w_rumbleIntensity = Intensity::MEDIUM;
			w_damage = 6 + ((level - 1) * 2);
			break;

		case SHOTGUN:
			w_shape.setSize(sf::Vector2f(100.f, 25.f));
			w_shape.setFillColor(sf::Color::Yellow);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 1.f - (0.05f * (level - 1));
			w_rumbleIntensity = Intensity::HIGH;
			w_damage = 15 + ((level - 1) * 5);
			break;

		// Melee
		case KNIFE:
			w_shape.setSize(sf::Vector2f(50.f, 50.f));
			w_shape.setFillColor(sf::Color::Blue);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.25f;
			w_rumbleIntensity = Intensity::LOW;
			break;

		case SWORD:
			w_shape.setSize(sf::Vector2f(50.f, 50.f));
			w_shape.setFillColor(sf::Color::Green);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.4f;
			w_rumbleIntensity = Intensity::MEDIUM;
			break;

		case AXE:
			w_shape.setSize(sf::Vector2f(50.f, 50.f));
			w_shape.setFillColor(sf::Color::Red);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.6f;
			w_rumbleIntensity = Intensity::HIGH;
			break;
	}

	w_shape.setOrigin({ w_shape.getSize().x / 4.f, w_shape.getSize().y / 2.f });

	w_cooldownRemaining = 0.f;

	if (type < WeaponType::KNIFE)
		w_isMelee = false;
	else
		w_isMelee = true;
}

/**
 * @brief Updates weapon positioning and cooldown timers.
 * @param dt Delta time in seconds.
 * @param playerPos World position to attach the weapon to.
 * @param aimDir Aim direction (world-space).
 */
void Weapon::update(float dt, const sf::Vector2f playerPos, const sf::Vector2f aimDir)
{
	handlePositioning(playerPos, aimDir);

	if (w_cooldownRemaining > 0.0f)
		w_cooldownRemaining -= dt;
}

/**
 * @brief Renders the weapon body shape.
 * @param window Render target.
 */
void Weapon::render(sf::RenderWindow& window)
{
	window.draw(w_shape);
}


/**
 * @brief Positions and rotates the weapon based on an aim direction.
 * @param playerPos Attachment origin.
 * @param aimDir Aim direction.
 */
void Weapon::handlePositioning(const sf::Vector2f playerPos, const sf::Vector2f aimDir)
{
	sf::Vector2f position = playerPos;
	w_shape.setPosition(position);

	float angle = std::atan2(aimDir.y, aimDir.x);
	w_shape.setRotation(sf::radians(angle));
}

/**
 * @brief Attempts to fire a ranged weapon into a projectile list.
 * @param req Fire request (aim direction + ownership).
 * @param projectileList Output list for spawned projectiles.
 */
void Weapon::fire(const FireReq& req, std::vector<std::unique_ptr<Projectile>>&projectileList)
{
	if (w_cooldownRemaining > 0.0f || req.aimDir == sf::Vector2f(0.f, 0.f))
		return;

	sf::Vector2f muzzleLocalPos = sf::Vector2f(w_shape.getSize().x, w_shape.getSize().y / 2.f);
	w_spawnPos = w_shape.getTransform().transformPoint(muzzleLocalPos);
	
	switch (w_fireMode) 
	{
	case FireMode::AUTOMATIC:
		spawnProjectile(req, projectileList);
		w_cooldownRemaining = w_fireRate;
		break;

	case FireMode::BURST:
		break;
	}
}

/**
 * @brief Attempts to fire a melee weapon into a damage-trigger list.
 * @param req Fire request (aim direction + ownership).
 * @param damageTriggerList Output list for spawned damage triggers.
 */
void Weapon::fire(const FireReq& req, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList)
{
	if (w_cooldownRemaining > 0.0f || req.aimDir == sf::Vector2f(0.f, 0.f))
		return;

	sf::Vector2f attackLocalPos = sf::Vector2f(w_shape.getSize().x, w_shape.getSize().y / 2.f);
	w_spawnPos = w_shape.getTransform().transformPoint(attackLocalPos);

	switch (w_fireMode)
	{
		case FireMode::AUTOMATIC:
			spawnMeleeHit(req, damageTriggerList);
			w_cooldownRemaining = w_fireRate;
			break;
		case FireMode::BURST:
			break;
	}
}