#include <cmath>
#include <random>
#include "NormalBulletProjectile.h"
#include "PlayerWeapons.h"

void PistolWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 550.0f;
	int damage = 10;
	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, damage, info.isFromPlayer));
}

void ARWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 600.f;
	int damage = 8;
	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, damage, info.isFromPlayer));
}

void ShotgunWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 500.f;
	int damage = 5;

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