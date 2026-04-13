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
}

/**
 * @brief Renders the chest.
 * @param window Render target.
 */
void Chest::render(sf::RenderWindow & window) const
{
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
