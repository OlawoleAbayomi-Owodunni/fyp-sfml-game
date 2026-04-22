#include "Chest.h"

/**
 * @file Chest.cpp
 * @brief Implements a simple chest entity used to provide rewards.
 */

/**
 * @brief Constructs a chest with a simple colored rectangle visual.
 * @param worldPos World position for the chest.
 * @param tileSize Base tile size used for scaling.
 */
Chest::Chest(const sf::Vector2f& worldPos, float tileSize)
{
	c_shape.setSize({ tileSize, tileSize });
	c_shape.setOrigin(c_shape.getSize() / 2.f);
	c_shape.setPosition(worldPos);
	c_shape.setFillColor(sf::Color(139, 69, 19)); // Saddle Brown
	c_shape.setOutlineThickness(2.f);
	c_shape.setOutlineColor(sf::Color::Black);

	if (!c_sprite.configure("ASSETS/SPRITES/Everything.png", {}, sf::Vector2i(0, 0)))
		c_sprite.configure("ASSETS/SPRITES/LEVELS/Job Board.png", {}, sf::Vector2i(0, 0));

	if (c_sprite.isLoaded())
	{
		const sf::FloatRect spriteBounds = c_sprite.getGlobalBounds();
		if (spriteBounds.size.x > 0.f && spriteBounds.size.y > 0.f)
		{
			c_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
			c_sprite.setScale(sf::Vector2f(
				c_shape.getSize().x / spriteBounds.size.x,
				c_shape.getSize().y / spriteBounds.size.y));
		}
		c_sprite.setPosition(c_shape.getPosition());
	}
}

/**
 * @brief Renders the chest.
 * @param window Render target.
 */
void Chest::render(sf::RenderWindow & window, bool texturedMode) const
{
	if (texturedMode && c_sprite.isLoaded())
		c_sprite.draw(window);
	else
		window.draw(c_shape);
}

/**
 * @brief Opens the chest and updates its visual state.
 */
void Chest::open()
{
	if (!c_isOpened)
	{
		c_isOpened = true;
		c_shape.setFillColor(sf::Color(90, 45, 10)); // Dark Brown
	}
}
