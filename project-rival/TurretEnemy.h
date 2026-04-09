#pragma once
#include "Enemy.h"

class TurretEnemy : public Enemy
{
public:
	TurretEnemy(const sf::Vector2f pos, int totalHealth);
	~TurretEnemy() = default;

	void init() override;
	void update(float dt) override;
	void setTarget(const Vector2f& target) override;

	void hitWall() override;

private:

};

