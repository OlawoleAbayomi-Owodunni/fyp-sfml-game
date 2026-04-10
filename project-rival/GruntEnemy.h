#pragma once
#include "Enemy.h"
#include "EnemyWeapon.h"

class GruntEnemy : public Enemy
{
public: 
	GruntEnemy(const sf::Vector2f pos, int totalHealth);
	~GruntEnemy() = default;

	void init() override;
	void update(float dt) override;
	void update(float dt, std::vector<std::unique_ptr<DamageTrigger>>& instantiableTriggers);
	void setTarget(const Vector2f& target) override;

	void hitWall() override;

private:
	SteeringAgent e_agent;
	SeekBehaviour e_seek{ Vector2f(0.f,0.f) };
	ArriveBehaviour e_arrive{ Vector2f(0.f,0.f), 500.f, 250.f };
	ISteeringBehaviour* e_currBehaviour{ nullptr };

	float e_arriveEnterDist;
	float e_arriveExitDist;

	EnemyWeapon ge_weapon{ WeaponType::KNIFE };
	float ge_attackTimer;
};

