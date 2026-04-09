#pragma once
#include "Weapon.h"

class PistolWeapon : public Weapon
{
	virtual void spawnProjectile(const  FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;
};

class ARWeapon: public Weapon
{
	virtual void spawnProjectile(const  FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;

};

class ShotgunWeapon :public Weapon
{
	virtual void spawnProjectile(const  FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;

};

class BurstRifleWeapon : public Weapon
{
	virtual void spawnProjectile(const  FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;

};