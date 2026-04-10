#pragma once
#include "Weapon.h"
class EnemyWeapon : public Weapon
{
public:
	EnemyWeapon(WeaponType type);

protected:
	void spawnProjectile(const FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;
	void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override;

private:
	int ew_damage;
};

