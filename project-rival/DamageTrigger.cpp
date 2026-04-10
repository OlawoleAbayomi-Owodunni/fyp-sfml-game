#include "DamageTrigger.h"

DamageTrigger::DamageTrigger(const sf::Vector2f& center, const sf::Vector2f& size, float lifetime, int damage, uint32_t targetMask)
	:dt_lifetime(lifetime), dt_shouldDestroy(false), dt_damage(damage)
{
	initShape(center, size);
	dt_collisionProfile.layer = CollisionLayer::DAMAGE_TRIGGER_LAYER;
	dt_collisionProfile.mask = targetMask;
}

void DamageTrigger::update(float dt)
{
	dt_lifetime -= dt;
	if (dt_lifetime <= 0.0f)
	{
		dt_shouldDestroy = true;
	}
}

void DamageTrigger::render(sf::RenderWindow& window)
{
	window.draw(dt_shape);
}

void DamageTrigger::initShape(const sf::Vector2f& center, const sf::Vector2f& size)
{
	dt_shape.setSize(size);
	dt_shape.setOrigin(size / 2.0f);
	dt_shape.setPosition(center);
	dt_shape.setFillColor(sf::Color(255, 0, 0, 70));

	dt_bounds = dt_shape.getGlobalBounds();
}
