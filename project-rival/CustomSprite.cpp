#include "CustomSprite.h"

namespace
{
	bool isValidTileSize(const sf::Vector2i& tileSize)
	{
		return tileSize.x > 0 && tileSize.y > 0;
	}
}

CustomSprite::CustomSprite(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize)
{
	configure(texturePath, animations, tileCutoutSize);
}

bool CustomSprite::configure(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize)
{
	m_loaded = false;
	m_animations = animations;
	m_tileCutoutSize = tileCutoutSize;
	m_currentAnimationName.clear();
	m_currentFrameIndex = 0;
	m_frameTimerSeconds = 0.f;
	m_sprite.reset();

	if (!m_texture.loadFromFile(texturePath))
		return false;

	m_sprite.emplace(m_texture);
	m_loaded = true;

	if (!hasAnimations())
	{
		const sf::Vector2u textureSize = m_texture.getSize();
		m_sprite->setTextureRect(sf::IntRect(
			sf::Vector2i(0, 0),
			sf::Vector2i(static_cast<int>(textureSize.x), static_cast<int>(textureSize.y))));
		return true;
	}

	if (!isValidTileSize(m_tileCutoutSize))
		return false;

	m_currentAnimationName = m_animations.begin()->first;
	applyCurrentFrame();
	return true;
}

void CustomSprite::update(float dtSeconds)
{
	if (!isLoaded() || !hasAnimations())
		return;

	auto animationIt = m_animations.find(m_currentAnimationName);
	if (animationIt == m_animations.end())
		return;

	const SpriteAnimationClip& clip = animationIt->second;
	if (clip.frameTiles.empty() || clip.fps <= 0.f)
		return;

	const float frameDurationSeconds = 1.f / clip.fps;
	m_frameTimerSeconds += dtSeconds;

	while (m_frameTimerSeconds >= frameDurationSeconds)
	{
		m_frameTimerSeconds -= frameDurationSeconds;

		if (m_currentFrameIndex + 1 < clip.frameTiles.size())
		{
			++m_currentFrameIndex;
		}
		else if (clip.loop)
		{
			m_currentFrameIndex = 0;
		}

		applyCurrentFrame();
	}
}

void CustomSprite::draw(sf::RenderWindow& window) const
{
	if (m_sprite.has_value())
		window.draw(*m_sprite);
}

bool CustomSprite::setAnimation(const std::string& animationName, bool restartIfSame)
{
	auto animationIt = m_animations.find(animationName);
	if (animationIt == m_animations.end())
		return false;

	if (!restartIfSame && m_currentAnimationName == animationName)
		return true;

	m_currentAnimationName = animationName;
	m_currentFrameIndex = 0;
	m_frameTimerSeconds = 0.f;
	applyCurrentFrame();
	return true;
}

void CustomSprite::setPosition(const sf::Vector2f& position)
{
	if (m_sprite.has_value())
		m_sprite->setPosition(position);
}

void CustomSprite::setOrigin(const sf::Vector2f& origin)
{
	if (m_sprite.has_value())
		m_sprite->setOrigin(origin);
}

void CustomSprite::setScale(const sf::Vector2f& scale)
{
	if (m_sprite.has_value())
		m_sprite->setScale(scale);
}

void CustomSprite::setRotation(sf::Angle rotation)
{
	if (m_sprite.has_value())
		m_sprite->setRotation(rotation);
}

void CustomSprite::setColor(const sf::Color& color)
{
	if (m_sprite.has_value())
		m_sprite->setColor(color);
}

bool CustomSprite::hasAnimation(const std::string& animationName) const
{
	return m_animations.find(animationName) != m_animations.end();
}

sf::FloatRect CustomSprite::getGlobalBounds() const
{
	if (!m_sprite.has_value())
		return sf::FloatRect();

	return m_sprite->getGlobalBounds();
}

void CustomSprite::applyCurrentFrame()
{
	if (!m_sprite.has_value())
		return;

	auto animationIt = m_animations.find(m_currentAnimationName);
	if (animationIt == m_animations.end())
		return;

	const SpriteAnimationClip& clip = animationIt->second;
	if (clip.frameTiles.empty())
		return;

	if (m_currentFrameIndex >= clip.frameTiles.size())
		m_currentFrameIndex = 0;

	const sf::Vector2i tile = clip.frameTiles[m_currentFrameIndex];
	const int left = tile.x * m_tileCutoutSize.x;
	const int top = tile.y * m_tileCutoutSize.y;

	m_sprite->setTextureRect(sf::IntRect(
		sf::Vector2i(left, top),
		sf::Vector2i(m_tileCutoutSize.x, m_tileCutoutSize.y)));
}
