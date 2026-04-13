#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Projectile.h"
#include "DamageTrigger.h"

/**
 * @file Weapon.h
 * @brief Declares weapon base class and shared weapon data types.
 */

/**
 * @brief Weapon archetypes supported by the player and enemies.
 */
enum WeaponType
{
	// Guns
	PISTOL,
	ASSAULT_RIFLE,
	SHOTGUN,

	// Melee
	KNIFE,
	SWORD,
	AXE,
	
	COUNT
};

/**
 * @brief Firing behaviour for weapons that support different cadence.
 */
enum FireMode
{
	AUTOMATIC,
	BURST
};

/**
 * @brief Controller rumble intensity used by weapons.
 */
enum Intensity
{
	LOW,
	MEDIUM,
	HIGH
};

/**
 * @brief Data passed to a weapon when attempting to fire.
 */
struct FireReq
{
	sf::Vector2f aimDir;
	bool isFromPlayer;
};

class Weapon
{
public:
	Weapon(WeaponType type, int level);
	virtual ~Weapon() = default;

	virtual void update(float dt, const sf::Vector2f playerPos, const sf::Vector2f aimDir);
	virtual void render(sf::RenderWindow& window);

	void fire(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList);
	void fire(const FireReq& req, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList);

	const bool isMelee() const { return w_isMelee; }
	const Intensity rumbleIntensity() const { return w_rumbleIntensity; }

protected:
	virtual void spawnProjectile(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList) = 0;
	virtual void spawnMeleeHit(const FireReq& req, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) = 0;

	sf::Vector2f w_spawnPos;
	int w_damage;

private:
	void init(WeaponType type);
	void handlePositioning(const sf::Vector2f playerPos, const sf::Vector2f aimDir);

	FireMode w_fireMode;

	int w_weaponLevel;
	float w_cooldownRemaining;
	float w_fireRate;

	sf::RectangleShape w_shape;

	FireReq w_fireReq;

	bool w_isMelee;

	Intensity w_rumbleIntensity;
};

