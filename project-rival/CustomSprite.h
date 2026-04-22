#pragma once

#include <SFML/graphics.hpp>

#include <optional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @file CustomSprite.h
 * @brief Sprite/animation wrapper that supports plain textures and tileset clips.
 */

/**
 * @brief Single animation clip configuration.
 */
struct SpriteAnimationClip
{
	std::vector<sf::Vector2i> frameTiles;
	float fps{ 0.f };
	bool loop{ true };
};

/**
 * @brief Single animation clip that stores explicit pixel rectangles.
 */
struct SpriteAnimationRectClip
{
	std::vector<sf::IntRect> frameRects;
	float fps{ 0.f };
	bool loop{ true };
};

/**
 * @brief Name -> clip mapping used by CustomSprite.
 */
using SpriteAnimationMap = std::unordered_map<std::string, SpriteAnimationClip>;
using SpriteAnimationRectMap = std::unordered_map<std::string, SpriteAnimationRectClip>;

class CustomSprite
{
public:
	CustomSprite() = default;
	CustomSprite(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize);

	bool configure(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize);
	bool configureWithRects(const std::string& texturePath, const SpriteAnimationRectMap& animations);

	void update(float dtSeconds);
	void draw(sf::RenderWindow& window) const;

	bool setAnimation(const std::string& animationName, bool restartIfSame = false);

	void setPosition(const sf::Vector2f& position);
	void setOrigin(const sf::Vector2f& origin);
	void setScale(const sf::Vector2f& scale);
	void setRotation(sf::Angle rotation);
	void setColor(const sf::Color& color);

	bool isLoaded() const { return m_loaded && m_sprite.has_value(); }
	bool hasAnimation(const std::string& animationName) const;

	sf::FloatRect getGlobalBounds() const;

private:
	void applyCurrentFrame();
	bool hasAnimations() const { return m_useRectAnimations ? !m_rectAnimations.empty() : !m_animations.empty(); }

	bool m_loaded{ false };
	std::shared_ptr<const sf::Texture> m_texture;
	std::optional<sf::Sprite> m_sprite;

	SpriteAnimationMap m_animations;
	SpriteAnimationRectMap m_rectAnimations;
	sf::Vector2i m_tileCutoutSize{ 0, 0 };
	bool m_useRectAnimations{ false };

	std::string m_currentAnimationName;
	std::size_t m_currentFrameIndex{ 0 };
	float m_frameTimerSeconds{ 0.f };
};
