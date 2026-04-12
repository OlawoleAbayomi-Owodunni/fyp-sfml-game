#include "Pickup.h"

Pickup::Pickup(PickupType type, const sf::Vector2f& worldPos, float tileSize)
	:p_type(type)
{
	initVisual(tileSize);
	p_shape.setPosition(worldPos);
}

void Pickup::render(sf::RenderWindow & window) const
{
	if (!p_shouldDestroy)
		window.draw(p_shape);
}

void Pickup::initVisual(float tileSize)
{
	float size = tileSize;

	switch (p_type)
	{
	case PickupType::HEALTH: {
		size = tileSize / 2.f;
		p_shape.setFillColor(sf::Color::Red);
		p_effectAmount = 5;
		break;
	}

	case PickupType::AMMO: {
		size = tileSize / 2.f;
		p_shape.setFillColor(sf::Color(0, 128, 0)); // Military Green
		p_effectAmount = 10;
		break;
	}

	case PickupType::SINGLE_COIN: {
		size = tileSize / 4.f;
		p_shape.setFillColor(sf::Color::Yellow);
		p_effectAmount = 5;
		break;
	}

	case PickupType::CHEST_COIN: {
		size = tileSize / 2.f;
		p_shape.setFillColor(sf::Color(255, 215, 0)); // Gold
		p_effectAmount = 50;
		break;
	}
	}

	p_shape.setSize({ size, size });
	p_shape.setOrigin(p_shape.getSize() / 2.f);
}
