#include <cmath>
#include <random>
#include "NormalBulletProjectile.h"
#include "PlayerWeapons.h"

/**
 * @file PlayerWeapons.cpp
 * @brief Implements player weapon-specific projectile and melee hit spawning.
 */

// ------------------------------- GUN WEAPONS -------------------------------
#pragma region GUN_WEAPONS
/**
 * @brief Spawns a pistol bullet with fixed speed.
 * @param info Fire request.
 * @param projectileList Output list for spawned projectiles.
 */
void PistolWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 550.0f;
	int damage = w_damage;
	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, damage, info.isFromPlayer));
}

/**
 * @brief Spawns an assault rifle bullet with fixed speed.
 * @param info Fire request.
 * @param projectileList Output list for spawned projectiles.
 */
void ARWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 600.f;
	int damage = w_damage;
	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, damage, info.isFromPlayer));
}

/**
 * @brief Spawns multiple shotgun pellets with random spread.
 * @param info Fire request.
 * @param projectileList Output list for spawned projectiles.
 */
void ShotgunWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 500.f;
	int damage = w_damage;

	// For shotgun, we want to spawn multiple projectiles in a spread pattern. E.G: spawn 5 projectiles with a spread of 30 degrees.
	float baseAngle = std::atan2(dir.y, dir.x);

	int pellets = 5;

	for (int i = 0; i < pellets; i++)
	{
		float randSpread = rand() % 30 - 15; // Random spread between -15 and 15 degrees
		float angle = baseAngle + (randSpread * 3.142f / 180.f);
		sf::Vector2f pelletDir(std::cos(angle), std::sin(angle));

		projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, pelletDir, speed, damage, info.isFromPlayer));
	}
}
#pragma endregion

// ------------------------------- MELEE WEAPONS -------------------------------
#pragma region MELEE_WEAPONS
/**
 * @brief Spawns a short-lived melee hit trigger for the knife.
 * @param info Fire request.
 * @param damageTriggerList Output list for spawned damage triggers.
 */
void KnifeWeapon::spawnMeleeHit(const FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	sf::Vector2f size = sf::Vector2f(50.f, 50.f);
	int damage = 20;

	sf::Vector2f center = w_spawnPos + dir * 30.f;

	damageTriggerList.push_back(std::make_unique<DamageTrigger>(
		center,
		size,
		0.1f,
		damage,
		info.isFromPlayer ? CollisionLayer::ENEMY_LAYER : CollisionLayer::PLAYER_LAYER
	));
}

/**
 * @brief Spawns a short-lived melee hit trigger for the sword.
 * @param info Fire request.
 * @param damageTriggerList Output list for spawned damage triggers.
 */
void SwordWeapon::spawnMeleeHit(const FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	sf::Vector2f size = sf::Vector2f(70.f, 70.f);
	int damage = 30;
	sf::Vector2f center = w_spawnPos + dir * 40.f;

	damageTriggerList.push_back(std::make_unique<DamageTrigger>(
		center,
		size,
		0.15f,
		damage,
		info.isFromPlayer ? CollisionLayer::ENEMY_LAYER : CollisionLayer::PLAYER_LAYER
	));
}

/**
 * @brief Spawns a short-lived melee hit trigger for the axe.
 * @param info Fire request.
 * @param damageTriggerList Output list for spawned damage triggers.
 */
void AxeWeapon::spawnMeleeHit(const FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	sf::Vector2f size = sf::Vector2f(100.f, 100.f);
	int damage = 100;
	sf::Vector2f center = w_spawnPos + dir * 50.f;

	damageTriggerList.push_back(std::make_unique<DamageTrigger>(
		center,
		size,
		0.2f,
		damage,
		info.isFromPlayer ? CollisionLayer::ENEMY_LAYER : CollisionLayer::PLAYER_LAYER
	));
}
#pragma endregion