#include "Pickup.h"

/**
 * @file Pickup.cpp
 * @brief Implements pickup visuals and effect amounts.
 */

/**
 * @brief Constructs a pickup at a world position.
 * @param type Pickup type.
 * @param worldPos World position for this pickup.
 * @param tileSize Base tile size for scaling.
 */
Pickup::Pickup(PickupType type, const sf::Vector2f& worldPos, float tileSize)
	:p_type(type)
{
	initVisual(tileSize);
	p_shape.setPosition(worldPos);

	SpriteAnimationRectMap pickupAnimations;
	const std::string pickupAtlasPath = "ASSETS/SPRITES/Everything.png";

	switch (p_type)
	{
	case PickupType::HEALTH:
		pickupAnimations.emplace("Idle", SpriteAnimationRectClip{ std::vector<sf::IntRect>{ sf::IntRect(sf::Vector2i(848, 582), sf::Vector2i(28, 48)) }, 1.f, true });
		break;
	case PickupType::AMMO:
		pickupAnimations.emplace("Idle", SpriteAnimationRectClip{ std::vector<sf::IntRect>{ sf::IntRect(sf::Vector2i(974, 587), sf::Vector2i(32, 46)) }, 1.f, true });
		break;
	case PickupType::SINGLE_COIN:
		pickupAnimations.emplace("Idle", SpriteAnimationRectClip{ std::vector<sf::IntRect>{ sf::IntRect(sf::Vector2i(986, 292), sf::Vector2i(32, 32)) }, 1.f, true });
		break;
	case PickupType::CHEST_COIN:
		pickupAnimations.emplace("Idle", SpriteAnimationRectClip{ std::vector<sf::IntRect>{ sf::IntRect(sf::Vector2i(986, 292), sf::Vector2i(32, 32)) }, 1.f, true });
		break;
	default:
		break;
	}

	if (!p_sprite.configureWithRects(pickupAtlasPath, pickupAnimations))
		p_sprite.configure("ASSETS/SPRITES/UI/hud.png", {}, sf::Vector2i(0, 0));

	if (p_sprite.isLoaded())
	{
		const sf::FloatRect spriteBounds = p_sprite.getGlobalBounds();
		if (spriteBounds.size.x > 0.f && spriteBounds.size.y > 0.f)
		{
			p_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
			p_sprite.setScale(sf::Vector2f(
				p_shape.getSize().x / spriteBounds.size.x,
				p_shape.getSize().y / spriteBounds.size.y));
		}
		p_sprite.setPosition(p_shape.getPosition());
	}
}

/**
 * @brief Renders the pickup if it has not been collected/destroyed.
 * @param window Render target.
 */
void Pickup::render(sf::RenderWindow & window, bool texturedMode) const
{
	if (!p_shouldDestroy)
	{
		if (texturedMode && p_sprite.isLoaded())
			p_sprite.draw(window);
		else
			window.draw(p_shape);
	}
}

/**
 * @brief Initializes the pickup shape and effect amounts based on type.
 * @param tileSize Base tile size for scaling.
 */
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
