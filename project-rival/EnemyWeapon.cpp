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
	}
}

void EnemyWeapon::spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList)
{
	sf::Vector2f dir = info.aimDir.normalized();
	float speed = 400.f;

	projectileList.push_back(std::make_unique<NormalBulletProjectile>(w_spawnPos, dir, speed, ew_damage, info.isFromPlayer));
}
