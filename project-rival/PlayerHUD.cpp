#include "PlayerHUD.h"

PlayerHUD::PlayerHUD(const sf::Font& font)
	: m_healthText(font), m_ammoText(font), m_coinsText(font)
{
	init(font);
}

void PlayerHUD::init(const sf::Font& font)
{
	// Health Bar
	m_healthBarBg.setSize({ phud_barWidth, phud_barHeight });
	m_healthBarBg.setPosition({ phud_marginLeft, phud_marginTop });
	m_healthBarBg.setFillColor(sf::Color(40, 40, 40)); // Dark gray background
	m_healthBarBg.setOutlineThickness(2.f);
	m_healthBarBg.setOutlineColor(sf::Color::White);

	m_healthBar.setSize({ phud_barWidth, phud_barHeight });
	m_healthBar.setPosition(m_healthBarBg.getPosition());
	m_healthBar.setFillColor(sf::Color(200, 40, 40)); // Red health bar

	m_healthText.setFont(font);
	m_healthText.setCharacterSize(14);
	m_healthText.setFillColor(sf::Color::White);

	// Ammo Bar
	m_ammoBarBg.setSize({ phud_barWidth, phud_barHeight });
	m_ammoBarBg.setPosition({ phud_marginLeft, phud_marginTop + phud_barHeight + phud_spacing });
	m_ammoBarBg.setFillColor(sf::Color(40, 40, 40)); // Dark gray background
	m_ammoBarBg.setOutlineThickness(2.f);
	m_ammoBarBg.setOutlineColor(sf::Color::White);

	m_ammoBar.setSize({ phud_barWidth, phud_barHeight });
	m_ammoBar.setPosition(m_ammoBarBg.getPosition());
	m_ammoBar.setFillColor(sf::Color(40, 200, 40)); // Green ammo bar

	m_ammoText.setFont(font);
	m_ammoText.setCharacterSize(14);
	m_ammoText.setFillColor(sf::Color::White);

	// Coins Text
	m_coinsText.setFont(font);
	m_coinsText.setCharacterSize(14);
	m_coinsText.setFillColor(sf::Color::Yellow);
	// place coin on the right side of the screen
}

void PlayerHUD::update(int currentHealth, int maxHealth, int currentAmmo, int maxAmmo, int coins)
{
	// Update health bar
	const float healthPercent = static_cast<float>(currentHealth) / maxHealth;
	m_healthBar.setSize({ phud_barWidth * healthPercent, phud_barHeight });
	m_healthText.setString("Health: " + std::to_string(currentHealth) + " / " + std::to_string(maxHealth));
	centerText(m_healthText, m_healthBarBg);

	// Update ammo bar
	const float ammoPercent = static_cast<float>(currentAmmo) / maxAmmo;
	m_ammoBar.setSize({ phud_barWidth * ammoPercent, phud_barHeight });
	m_ammoText.setString("Ammo: " + std::to_string(currentAmmo) + " / " + std::to_string(maxAmmo));
	centerText(m_ammoText, m_ammoBarBg);

	// Update coins text
	m_coinsText.setString("Coins: " + std::to_string(coins));
	m_coinsText.setPosition({ phud_marginLeft, phud_marginTop + 2 * (phud_barHeight + phud_spacing) });
}

void PlayerHUD::render(sf::RenderWindow & window)
{
	window.draw(m_healthBarBg);
	window.draw(m_healthBar);
	window.draw(m_healthText);

	window.draw(m_ammoBarBg);
	window.draw(m_ammoBar);
	window.draw(m_ammoText);
	
	window.draw(m_coinsText);
}

void PlayerHUD::centerText(sf::Text & text, const sf::RectangleShape & rect)
{
	const sf::FloatRect textBounds = text.getLocalBounds();

	text.setOrigin({
		textBounds.position.x + textBounds.size.x / 2.f,
				textBounds.position.y + textBounds.size.y / 2.f
		});

	const sf::Vector2f rectPos = rect.getPosition();
	const sf::Vector2f rectSize = rect.getSize();

	text.setPosition({
		rectPos.x + rectSize.x / 2.f,
		rectPos.y + rectSize.y / 2.f
		});
}
