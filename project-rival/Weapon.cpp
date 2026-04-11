#include "Weapon.h"
#include <cmath>

Weapon::Weapon(WeaponType type, int level) : w_weaponLevel(level)
{
	init(type);
}

void Weapon::init(WeaponType type)
{
	const int level = w_weaponLevel;
	WeaponType weaponType = type;
	switch(weaponType)
	{
		// Guns
		case PISTOL:
			w_shape.setSize(sf::Vector2f(80.f, 20.f));
			w_shape.setFillColor(sf::Color::Blue);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.5f + (0.05f * (level - 1));
			w_rumbleIntensity = Intensity::LOW;
			w_damage = 10 + ((level - 1) * 4);
			break;

		case ASSAULT_RIFLE:
			w_shape.setSize(sf::Vector2f(120.f, 20.f));
			w_shape.setFillColor(sf::Color::Red);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.1f + (0.02f * (level - 1));
			w_rumbleIntensity = Intensity::MEDIUM;
			w_damage = 6 + ((level - 1) * 2);
			break;

		case SHOTGUN:
			w_shape.setSize(sf::Vector2f(100.f, 25.f));
			w_shape.setFillColor(sf::Color::Yellow);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 1.f + (0.04f * (level - 1));
			w_rumbleIntensity = Intensity::HIGH;
			w_damage = 15 + ((level - 1) * 5);
			break;

		// Melee
		case KNIFE:
			w_shape.setSize(sf::Vector2f(50.f, 50.f));
			w_shape.setFillColor(sf::Color::Blue);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.25f;
			w_rumbleIntensity = Intensity::LOW;
			break;

		case SWORD:
			w_shape.setSize(sf::Vector2f(50.f, 50.f));
			w_shape.setFillColor(sf::Color::Green);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.4f;
			w_rumbleIntensity = Intensity::MEDIUM;
			break;

		case AXE:
			w_shape.setSize(sf::Vector2f(50.f, 50.f));
			w_shape.setFillColor(sf::Color::Red);
			w_fireMode = FireMode::AUTOMATIC;
			w_fireRate = 0.6f;
			w_rumbleIntensity = Intensity::HIGH;
			break;
	}

	w_shape.setOrigin({ w_shape.getSize().x / 4.f, w_shape.getSize().y / 2.f });

	w_cooldownRemaining = 0.f;

	if (type < WeaponType::KNIFE)
		w_isMelee = false;
	else
		w_isMelee = true;
}

void Weapon::update(float dt, const sf::Vector2f playerPos, const sf::Vector2f aimDir)
{
	handlePositioning(playerPos, aimDir);

	if (w_cooldownRemaining > 0.0f)
		w_cooldownRemaining -= dt;
}

void Weapon::render(sf::RenderWindow& window)
{
	window.draw(w_shape);
}


void Weapon::handlePositioning(const sf::Vector2f playerPos, const sf::Vector2f aimDir)
{
	sf::Vector2f position = playerPos;
	w_shape.setPosition(position);

	float angle = std::atan2(aimDir.y, aimDir.x);
	w_shape.setRotation(sf::radians(angle));
}

void Weapon::fire(const FireReq& req, std::vector<std::unique_ptr<Projectile>>&projectileList)
{
	if (w_cooldownRemaining > 0.0f || req.aimDir == sf::Vector2f(0.f, 0.f))
		return;

	sf::Vector2f muzzleLocalPos = sf::Vector2f(w_shape.getSize().x, w_shape.getSize().y / 2.f);
	w_spawnPos = w_shape.getTransform().transformPoint(muzzleLocalPos);
	
	switch (w_fireMode) 
	{
	case FireMode::AUTOMATIC:
		spawnProjectile(req, projectileList);
		w_cooldownRemaining = w_fireRate;
		break;

	case FireMode::BURST:
		break;
	}
}

void Weapon::fire(const FireReq& req, std::vector<std::unique_ptr<DamageTrigger>>& damageTriggerList)
{
	if (w_cooldownRemaining > 0.0f || req.aimDir == sf::Vector2f(0.f, 0.f))
		return;

	sf::Vector2f attackLocalPos = sf::Vector2f(w_shape.getSize().x, w_shape.getSize().y / 2.f);
	w_spawnPos = w_shape.getTransform().transformPoint(attackLocalPos);

	switch (w_fireMode)
	{
		case FireMode::AUTOMATIC:
			spawnMeleeHit(req, damageTriggerList);
			w_cooldownRemaining = w_fireRate;
			break;
		case FireMode::BURST:
			break;
	}
}