#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Projectile.h"

enum WeaponType
{
	PISTOL,
	ASSAULT_RIFLE,
	SHOTGUN,
	
	COUNT
};

enum FireMode
{
	AUTOMATIC,
	BURST
};

struct FireReq
{
	sf::Vector2f aimDir;
	bool isFromPlayer;
};

class Weapon
{
public:
	Weapon(WeaponType type);
	virtual ~Weapon() = default;

	virtual void update(float dt, const sf::Vector2f playerPos, const sf::Vector2f aimDir);
	virtual void render(sf::RenderWindow& window);

	void fire(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList);


protected:
	virtual void spawnProjectile(const FireReq& req, std::vector<std::unique_ptr<Projectile>>& projectileList) = 0;

	sf::Vector2f w_spawnPos;

private:
	void init(WeaponType type);
	void handlePositioning(const sf::Vector2f playerPos, const sf::Vector2f aimDir);

	FireMode w_fireMode;

	float w_cooldownRemaining;
	float w_fireRate;

	sf::RectangleShape w_shape;

	FireReq w_fireReq;
};

