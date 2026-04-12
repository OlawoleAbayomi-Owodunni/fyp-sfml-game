#pragma once
#include <SFML/Graphics.hpp>

#include "Collision.h"
#include "Weapon.h"

struct WeaponInLoadout
{
	WeaponType type;
	int level;
};

class Player : public ICollidable
{
public:
	Player();
	~Player();

	void init();
	void update(float dt, 
		const sf::Vector2f& mouseWorldPos,
		const sf::Vector2i& mousePixelPos,
		std::vector<std::unique_ptr<Projectile>>& gameProjectiles, 
		std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers);
	void render(sf::RenderWindow& window);

	const sf::Vector2f getPosition() const { return p_body.getPosition(); }
	sf::FloatRect getCollisionBounds() const override { return p_body.getGlobalBounds(); }
	CollisionProfile getCollisionProfile() const override { return p_collisionProfile; }
	const std::vector<WeaponInLoadout>& getWeaponLoadout() const { return p_weaponLoadout; }
	int getCurrentWeaponID() const { return p_currentWeaponID; }
	int getHealth() const { return p_health; }
	int getMaxHealth() const { return p_maxHealth; }
	int getAmmo() const { return p_playerAmmo; }
	int getMaxAmmo() const { return p_maxAmmo; }

	void setSpawnPosition(const sf::Vector2f& spawnPos) { p_body.setPosition(spawnPos); }
	void setBodyColor(const sf::Color& color) { p_body.setFillColor(color); }
	void setCurrentWeaponID(int id) { if (id >= 0 && id < p_weapons.size()) p_currentWeaponID = id; }

	void hitWall();

	void takeDamage(int damage);
	const bool isDead() const { return p_isDead; }

	void applyUpgrade(int healthLevel, int speedLevel, int ammoLevel);

	bool addWeaponToLoadout(WeaponType type, int level);
	bool dropWeaponFromLoadout(int slotIndex);
	bool swapCurrentWeapon(WeaponType type, int level, WeaponInLoadout& weaponToDrop);
	void clearLoadout() { p_weapons.clear(); p_weaponLoadout.clear(); p_currentWeaponID = 0; }


private:
	// FUNCTIONS
	void startUp();
	void reset();
	void handleMovement(float dt);
	void handleAiming(const sf::Vector2f& mouseWorldPos, const sf::Vector2i& mousePixelPos);
	void ManageWeapons(std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers, std::vector<std::unique_ptr<Projectile>>& gameProjectiles, float dt);


	void buildWeaponsFromLoadout();
	std::unique_ptr<Weapon> createWeapon(WeaponType type, int level);

	// VARIABLES
	sf::RectangleShape p_body;

	sf::Vector2f p_velocity{ 0.f, 0.f };

	sf::Vector2f p_prevPos;

	bool p_isController;

	// reticle
	sf:: RectangleShape p_reticle;
	sf::Vector2f p_aimDir{ 0.f,0.f };
	float p_reticleDistance;
	sf::Vector2f p_prevMousePos;
	sf::Vector2i p_prevMousePixelPos;

	// collsion
	CollisionProfile p_collisionProfile;

	// entity stats
	int p_maxHealth;
	int p_health;
	bool p_isDead;
	int p_maxAmmo;
	int p_playerAmmo;
	float p_moveSpeed{ 200.f };

	// weapons
	static constexpr int MAX_WEAPONS = 3;
	std::vector<WeaponInLoadout> p_weaponLoadout;
	std::vector<std::unique_ptr<Weapon>> p_weapons;
	int p_currentWeaponID; 
	
	float p_rumbleTimer;
	float p_lowRumble;
	float p_highRumble;

};
