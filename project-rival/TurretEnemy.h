#pragma once
#include "Enemy.h"
#include "EnemyWeapon.h"

class TurretEnemy : public Enemy
{
public:
	TurretEnemy(const sf::Vector2f pos, int totalHealth);
	~TurretEnemy() = default;

	void init() override;
	void update(float dt) override;
	void update(float dt, std::vector<std::unique_ptr<Projectile>>& gameProjectiles);
	void render(sf::RenderWindow& window) override;

	void setTarget(const Vector2f& target) override;
	EnemyType getEnemyType() const override { return EnemyType::TURRET; }

	void hitWall() override;

private:
	EnemyWeapon te_weapon{ WeaponType::PISTOL };
	float te_fireTimer{ 0.f };
};

