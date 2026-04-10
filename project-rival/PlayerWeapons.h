#pragma once
#include "Weapon.h"

// ------------------------------- GUN WEAPONS -------------------------------
class PistolWeapon : public Weapon
{
public:
	PistolWeapon() : Weapon(WeaponType::PISTOL) {}
protected:
	virtual void spawnProjectile(const  FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;

private:
	virtual void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override {}
};

class ARWeapon: public Weapon
{
public:
	ARWeapon() : Weapon(WeaponType::ASSAULT_RIFLE) {}
protected:
	virtual void spawnProjectile(const  FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;

private:
	virtual void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override {}
};

class ShotgunWeapon : public Weapon
{
public:
	ShotgunWeapon() : Weapon(WeaponType::SHOTGUN) {}
protected:
	virtual void spawnProjectile(const  FireReq& info, std::vector<std::unique_ptr<Projectile>>& projectileList) override;

private:
	virtual void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override {}
};


// ------------------------------- MELEE WEAPONS -------------------------------
class KnifeWeapon : public Weapon
{
public:
	KnifeWeapon() : Weapon(WeaponType::KNIFE) {}
protected:
	virtual void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override;

private:
	virtual void spawnProjectile(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList) override {}
};

class SwordWeapon : public Weapon
{
public:
	SwordWeapon() : Weapon(WeaponType::SWORD) {}
protected:
	virtual void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override;

private:
	virtual void spawnProjectile(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList) override {}
};

class AxeWeapon : public Weapon
{
public:
	AxeWeapon() : Weapon(WeaponType::AXE) {}
protected:
	virtual void spawnMeleeHit(const  FireReq& info, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList) override;

private:
	virtual void spawnProjectile(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList) override {}
};