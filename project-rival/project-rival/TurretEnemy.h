#pragma once]
#include "Enemy.h"

class TurretEnemy : public Enemy
{
public:
	TurretEnemy(const sf::Vector2f pos);
	~TurretEnemy() = default;

	void init() override;
	void update(double dt) override;
	void setTarget(const Vector2f& target) override;

private:

};

