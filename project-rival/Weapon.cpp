#include "Weapon.h"

void Weapon::update(float dt)
{
	if (w_timeTillNextShot > 0.0f)
		w_timeTillNextShot -= dt;
}

const bool Weapon::canSpawn()
{
	if(w_shotsRemaining > 0 && w_timeTillNextShot <= 0.0f)
		return true;
	else
		return false;
}

void Weapon::fire(const FireInfo & fireInfo, std::vector<std::unique_ptr<Projectile>>&projectileList)
{
	w_currentFireInfo = fireInfo;
	if (w_currentFireInfo.shotsToFire < 1)
		return;
	else {
		w_shotsRemaining = w_currentFireInfo.shotsToFire;
	}

	while (canSpawn())
	{
		spawnProjectile(w_currentFireInfo, projectileList);
		w_shotsRemaining--;
		if (w_shotsRemaining > 0)
			w_timeTillNextShot = w_currentFireInfo.secondsBetweenShots;
		else
			w_timeTillNextShot = 0.0f;
	}
}