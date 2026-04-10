#include "EnemyWeapon.h"
#include "NormalBulletProjectile.h"

EnemyWeapon::EnemyWeapon(WeaponType type) : Weapon(type)
{
	switch (type)
	{
	case PISTOL:
		ew_damage = 10;
		break;
	case ASSAULT_RIFLE:
		ew_damage = 8;
		break;
	case SHOTGUN:
		ew_damage = 5;
		break;

	case KNIFE:
		ew_damage = 5;
		break;
	case SWORD:
		ew_damage = 15;
		break;
	case AXE:
		ew_damage = 25;
		break;
	}
}

void EnemyWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = static_cast<float>(rand() % 200 + 100);

	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, ew_damage, info.isFromPlayer));
}

void EnemyWeapon::spawnMeleeHit(const FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	sf::Vector2f size = sf::Vector2f(50.f, 50.f);
	sf::Vector2f center = w_spawnPos + dir * 30.f;

	damageTriggerList.push_back(std::make_unique<DamageTrigger>(
		center,
		size,
		0.1f,
		ew_damage,
		info.isFromPlayer ? CollisionLayer::ENEMY_LAYER : CollisionLayer::PLAYER_LAYER
	));
}
