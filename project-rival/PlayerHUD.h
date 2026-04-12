#pragma once
#include <SFML/Graphics.hpp>

class PlayerHUD
{
public:
	PlayerHUD(const sf::Font& font);
	void init(const sf::Font& font);
	void update(int currentHealth, int maxHealth, int currentAmmo, int maxAmmo, int coins);
	void render(sf::RenderWindow& window);

private:
	void centerText(sf::Text& text, const sf::RectangleShape& rect);

	sf::RectangleShape m_healthBarBg;
	sf::RectangleShape m_healthBar;
	sf::Text m_healthText;

	sf::RectangleShape m_ammoBarBg;
	sf::RectangleShape m_ammoBar;
	sf::Text m_ammoText;

	sf::Text m_coinsText;

	float phud_barWidth{ 200.f };
	float phud_barHeight{ 20.f };
	float phud_marginLeft{ 20.f };
	float phud_marginTop{ 20.f };
	float phud_spacing{ 10.f };
};

