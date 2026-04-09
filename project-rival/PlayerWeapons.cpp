#include <cmath>
#include <random>
#include "PlayerWeapons.h"

void PistolWeapon::spawnProjectile(const FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 50.0f;
	int damage = 10;
	projectileList.push_back(std::make_unique<Projectile>(info.spawnPos, dir, speed, damage, info.isFromPlayer));
}

void ARWeapon::spawnProjectile(const FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 70.0f;
	int damage = 8;
	projectileList.push_back(std::make_unique<Projectile>(info.spawnPos, dir, speed, damage, info.isFromPlayer));
}

void ShotgunWeapon::spawnProjectile(const FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();

	// For shotgun, we want to spawn multiple projectiles in a spread pattern. E.G: spawn 5 projectiles with a spread of 30 degrees.
	float speed = 40.0f;
	int damage = 5;

	float baseAngle = std::atan2(dir.y, dir.x);
	float randSpread = rand() % 30 - 15; // Random spread between -15 and 15 degrees

	int pellets = 5;

	for (int i = 0; i < pellets; i++)
	{
		float angle = baseAngle + (randSpread * 3.142f / 180.f);
		sf::Vector2f pelletDir(std::cos(angle), std::sin(angle));

		projectileList.push_back(std::make_unique<Projectile>(info.spawnPos, pelletDir, speed, damage, info.isFromPlayer));
	}
}

void BurstRifleWeapon::spawnProjectile(const FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 60.0f;
	int damage = 12;
	projectileList.push_back(std::make_unique<Projectile>(info.spawnPos, dir, speed, damage, info.isFromPlayer));
}
