#include "DamageTrigger.h"

/**
 * @file DamageTrigger.cpp
 * @brief Implements a timed hitbox that applies damage on overlap.
 */

/**
 * @brief Constructs a timed damage trigger.
 *
 * The trigger uses `CollisionLayer::DAMAGE_TRIGGER_LAYER` and a caller-provided
 * mask to control which entities can be affected.
 *
 * @param center World-space center of the trigger.
 * @param size World-space size of the trigger.
 * @param lifetime Lifetime in seconds before auto-destruction.
 * @param damage Damage dealt when overlapping.
 * @param targetMask Collision mask describing valid damage targets.
 */
DamageTrigger::DamageTrigger(const sf::Vector2f& center, const sf::Vector2f& size, float lifetime, int damage, uint32_t targetMask)
	:dt_lifetime(lifetime), dt_shouldDestroy(false), dt_damage(damage)
{
	initShape(center, size);
	dt_collisionProfile.layer = CollisionLayer::DAMAGE_TRIGGER_LAYER;
	dt_collisionProfile.mask = targetMask;

	SpriteAnimationRectMap triggerAnimations{
		{ "Idle", { std::vector<sf::IntRect>{
			sf::IntRect(sf::Vector2i(480, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(960, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(1440, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(1920, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(2400, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(2880, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(3360, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(3840, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(4320, 0), sf::Vector2i(48, 48)),
			sf::IntRect(sf::Vector2i(4800, 0), sf::Vector2i(48, 48))
		}, 16.f, false } }
	};

	if (!dt_sprite.configureWithRects("ASSETS/SPRITES/WEAPONS/60.png", triggerAnimations))
		dt_sprite.configure("ASSETS/SPRITES/UI/crosshair.png", {}, sf::Vector2i(0, 0));

	if (dt_sprite.isLoaded())
	{
		const sf::FloatRect spriteBounds = dt_sprite.getGlobalBounds();
		if (spriteBounds.size.x > 0.f && spriteBounds.size.y > 0.f)
		{
			dt_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
			dt_sprite.setScale(sf::Vector2f(
				dt_shape.getSize().x / spriteBounds.size.x,
				dt_shape.getSize().y / spriteBounds.size.y));
		}
		dt_sprite.setPosition(dt_shape.getPosition());
		dt_sprite.setColor(sf::Color(255, 255, 255, 120));
	}
}

/**
 * @brief Updates lifetime countdown and marks the trigger for destruction when expired.
 * @param dt Delta time in seconds.
 */
void DamageTrigger::update(float dt)
{
	dt_lifetime -= dt;
	dt_sprite.update(dt);
	if (dt_lifetime <= 0.0f)
	{
		dt_shouldDestroy = true;
	}
}

/**
 * @brief Renders the trigger debug shape.
 * @param window Render target.
 */
void DamageTrigger::render(sf::RenderWindow& window, bool texturedMode)
{
	if (texturedMode && dt_sprite.isLoaded())
		dt_sprite.draw(window);
	else
		window.draw(dt_shape);
}

/**
 * @brief Initializes the underlying SFML shape and cached bounds.
 * @param center World-space center.
 * @param size World-space size.
 */
void DamageTrigger::initShape(const sf::Vector2f& center, const sf::Vector2f& size)
{
	dt_shape.setSize(size);
	dt_shape.setOrigin(size / 2.0f);
	dt_shape.setPosition(center);
	dt_shape.setFillColor(sf::Color(255, 0, 0, 70));

	dt_bounds = dt_shape.getGlobalBounds();
}
