#pragma once
#include "Weapon.h"

class PistolWeapon : public Weapon
{
public:
	PistolWeapon() : Weapon(WeaponType::PISTOL) {}
	virtual void spawnProjectile(const  FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;
};

class ARWeapon: public Weapon
{
public:
	ARWeapon() : Weapon(WeaponType::ASSAULT_RIFLE) {}
	virtual void spawnProjectile(const  FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;
};

class ShotgunWeapon :public Weapon
{
public:
	ShotgunWeapon() : Weapon(WeaponType::SHOTGUN) {}
	virtual void spawnProjectile(const  FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;
};