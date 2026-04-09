#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Projectile.h"

//enum WeaponType
//{
//	PISTOL,
//	SHOTGUN,
//	RIFLE,
//	// Add more weapon types as needed
//	
//	COUNT
//};

struct FireInfo
{
	sf::Vector2f spawnPos;
	sf::Vector2f aimDir;
	bool isFromPlayer;
	
	int shotsToFire;
	float secondsBetweenShots;

	//WeaponType weaponType;
};

class Weapon
{
public:
	virtual ~Weapon() = default;

	virtual void update(float dt);

	void fire(const FireInfo& fireInfo, std::vector<std::unique_ptr<Projectile>>& projectileList);

protected:
	virtual void spawnProjectile(const FireInfo& info, std::vector<std::unique_ptr<Projectile>>& projectileList) = 0;

	sf::Vector2f normalise(const sf::Vector2f& direction);

private:
	const bool canSpawn();

	int w_shotsRemaining;
	float w_timeTillNextShot;
	FireInfo w_currentFireInfo;
};

