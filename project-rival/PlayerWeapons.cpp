#include <cmath>
#include <random>
#include "NormalBulletProjectile.h"
#include "PlayerWeapons.h"

// ------------------------------- GUN WEAPONS -------------------------------
#pragma region GUN_WEAPONS
void PistolWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 550.0f;
	int damage = 15;
	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, damage, info.isFromPlayer));
}

void ARWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 600.f;
	int damage = 10;
	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, damage, info.isFromPlayer));
}

void ShotgunWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 500.f;
	int damage = 25;

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