#include "PlayerHUD.h"
#include "Quest.h"

#include <unordered_map>

namespace
{
	const sf::Texture* loadTexture(const std::string& texturePath)
	{
		static std::unordered_map<std::string, sf::Texture> textureCache;
		auto [it, inserted] = textureCache.try_emplace(texturePath);
		if (inserted && !it->second.loadFromFile(texturePath))
			return nullptr;
		return &it->second;
	}
}

/**
 * @file PlayerHUD.cpp
 * @brief Implements the on-screen HUD for player health, ammo, coins, and quest progress.
 */

/**
 * @brief Constructs the HUD and initializes its visual elements.
 * @param font Font used for HUD text.
 */
PlayerHUD::PlayerHUD(const sf::Font& font)
	: m_healthText(font), m_ammoText(font), m_coinsText(font), m_questText(font)
{
	init(font);
}

/**
 * @brief Initializes shapes and text properties.
 * @param font Font used for HUD text.
 */
void PlayerHUD::init(const sf::Font& font)
{
   const float s = phud_scale;
	const float barWidth = phud_barWidth * s;
	const float barHeight = phud_barHeight * s;
	const float marginLeft = phud_marginLeft * s;
	const float marginTop = phud_marginTop * s;
	const float spacing = phud_spacing * s;

	// Health Bar
   m_healthBarBg.setSize({ barWidth, barHeight });
	m_healthBarBg.setPosition({ marginLeft, marginTop });
	m_healthBarBg.setFillColor(sf::Color(40, 40, 40)); // Dark gray background
	m_healthBarBg.setOutlineThickness(2.f);
	m_healthBarBg.setOutlineColor(sf::Color::White);

 m_healthBar.setSize({ barWidth, barHeight });
	m_healthBar.setPosition(m_healthBarBg.getPosition());
	m_healthBar.setFillColor(sf::Color(200, 40, 40)); // Red health bar

	m_healthText.setFont(font);
  m_healthText.setCharacterSize(static_cast<unsigned int>(14 * s));
	m_healthText.setFillColor(sf::Color::White);

	// Ammo Bar
 m_ammoBarBg.setSize({ barWidth, barHeight });
	m_ammoBarBg.setPosition({ marginLeft, marginTop + barHeight + spacing });
	m_ammoBarBg.setFillColor(sf::Color(40, 40, 40)); // Dark gray background
	m_ammoBarBg.setOutlineThickness(2.f);
	m_ammoBarBg.setOutlineColor(sf::Color::White);

   m_ammoBar.setSize({ barWidth, barHeight });
	m_ammoBar.setPosition(m_ammoBarBg.getPosition());
	m_ammoBar.setFillColor(sf::Color(40, 200, 40)); // Green ammo bar

	m_ammoText.setFont(font);
    m_ammoText.setCharacterSize(static_cast<unsigned int>(14 * s));
	m_ammoText.setFillColor(sf::Color::White);

	// Coins Text
	m_coinsText.setFont(font);
   m_coinsText.setCharacterSize(static_cast<unsigned int>(14 * s));
	m_coinsText.setFillColor(sf::Color::Yellow);
	// place coin on the right side of the screen

	// Quest tracker text
	m_questText.setFont(font);
   m_questText.setCharacterSize(static_cast<unsigned int>(14 * s));
	m_questText.setFillColor(sf::Color::White);
	m_questText.setOutlineThickness(1.f);
	m_questText.setOutlineColor(sf::Color::Black);
}

/**
 * @brief Updates HUD values and quest tracker display.
 * @param currentHealth Current health.
 * @param maxHealth Maximum health.
 * @param currentAmmo Current ammo.
 * @param maxAmmo Maximum ammo.
 * @param coins Current coin count.
 * @param activeQuest Active quest data or null when no quest is active.
 */
void PlayerHUD::update(int currentHealth, int maxHealth, int currentAmmo, int maxAmmo, int coins, const QuestData* activeQuest)
{
    const float s = phud_scale;
	const float barWidth = phud_barWidth * s;
	const float barHeight = phud_barHeight * s;
	const float marginLeft = phud_marginLeft * s;
	const float marginTop = phud_marginTop * s;
	const float spacing = phud_spacing * s;

	// Update health bar
	const float healthPercent = static_cast<float>(currentHealth) / maxHealth;
 m_healthBar.setSize({ barWidth * healthPercent, barHeight });
	m_healthText.setString("Health: " + std::to_string(currentHealth) + " / " + std::to_string(maxHealth));
	centerText(m_healthText, m_healthBarBg);

	// Update ammo bar
	const float ammoPercent = static_cast<float>(currentAmmo) / maxAmmo;
 m_ammoBar.setSize({ barWidth * ammoPercent, barHeight });
	m_ammoText.setString("Ammo: " + std::to_string(currentAmmo) + " / " + std::to_string(maxAmmo));
	centerText(m_ammoText, m_ammoBarBg);

	// Update coins text
	m_coinsText.setString("Coins: " + std::to_string(coins));
 m_coinsText.setPosition({ marginLeft, marginTop + 2 * (barHeight + spacing) });

	// Update quest tracker
	if (activeQuest)
	{
		const std::string progressText = std::to_string(activeQuest->progress) + " / " + std::to_string(activeQuest->targetCount);
		m_questText.setString("Quest: " + activeQuest->title + " (" + progressText + ")");
     m_questText.setPosition({ marginLeft, marginTop + 3 * (barHeight + spacing) });
	}
	else
	{
		m_questText.setString("");
	}
}

/**
 * @brief Draws the HUD to the window.
 * @param window Render target.
 */
void PlayerHUD::render(sf::RenderWindow & window)
{
	if (const sf::Texture* hudTexture = loadTexture("ASSETS/SPRITES/UI/hud.png"))
	{
		sf::Sprite hudPanel(*hudTexture);
		const sf::FloatRect panelBounds = hudPanel.getGlobalBounds();
		const sf::Vector2f targetSize(420.f, 180.f);
		if (panelBounds.size.x > 0.f && panelBounds.size.y > 0.f)
		{
			hudPanel.setScale(sf::Vector2f(targetSize.x / panelBounds.size.x, targetSize.y / panelBounds.size.y));
			hudPanel.setPosition(sf::Vector2f(10.f, 10.f));
			window.draw(hudPanel);
		}
	}

	window.draw(m_healthBarBg);
	window.draw(m_healthBar);
	window.draw(m_healthText);

	window.draw(m_ammoBarBg);
	window.draw(m_ammoBar);
	window.draw(m_ammoText);
	
	window.draw(m_coinsText);
	if (!m_questText.getString().isEmpty())
		window.draw(m_questText);
}

/**
 * @brief Centers text within a rectangle shape.
 * @param text Text to center.
 * @param rect Rectangle used as the reference bounds.
 */
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
