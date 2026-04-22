#include "Projectile.h"

#include <cmath>

/**
 * @file Projectile.cpp
 * @brief Implements base projectile state such as collision profile, lifetime, and rendering.
 */

/**
 * @brief Constructs a projectile with movement parameters and collision filtering.
 *
 * The collision layer is selected based on @p isFromPlayer and the mask is
 * configured to collide with the appropriate opposing entities plus environment.
 *
 * @param spawnPoint Initial world position.
 * @param direction Direction of travel (may be normalized by derived types).
 * @param speed Units per second.
 * @param damage Damage dealt on hit.
 * @param isFromPlayer True when spawned by the player.
 */
Projectile::Projectile(sf::Vector2f spawnPoint, sf::Vector2f direction, float speed, int damage, bool isFromPlayer) :
	p_spawnPoint(spawnPoint), p_direction(direction), p_speed(speed), p_damage(damage), p_isFromPlayer(isFromPlayer)
{
	init_body();

	// Atlas rects are grouped by ownership so player/enemy shots stay visually distinct.
	SpriteAnimationRectMap bulletAnimations;
	if (isFromPlayer)
	{
		bulletAnimations.emplace("Idle", SpriteAnimationRectClip{
			std::vector<sf::IntRect>{
				sf::IntRect(sf::Vector2i(976, 208), sf::Vector2i(32, 32))
			},
			1.f,
			true
		});
	}
	else
	{
		bulletAnimations.emplace("Idle", SpriteAnimationRectClip{
			std::vector<sf::IntRect>{
				sf::IntRect(sf::Vector2i(912, 208), sf::Vector2i(32, 32))
			},
			1.f,
			true
		});
	}

	if (!configureSpriteRects("ASSETS/SPRITES/Everything.png", bulletAnimations))
		configureSprite("ASSETS/SPRITES/UI/crosshair.png", {}, sf::Vector2i(0, 0));

	if(isFromPlayer)
	{
		p_collisionProfile.layer = CollisionLayer::PLAYER_BULLET_LAYER;
		p_collisionProfile.mask = CollisionLayer::ENEMY_LAYER;
	}
	else
	{
		p_collisionProfile.layer = CollisionLayer::ENEMY_BULLET_LAYER;
		p_collisionProfile.mask = CollisionLayer::PLAYER_LAYER;
	}
	// add other collision masks for environment (e.g. walls)
	// like so ----> p_collisionProfile.mask |= CollisionLayer::LEVEL_LAYER;
	p_collisionProfile.mask |= CollisionLayer::WALL_LAYER | CollisionLayer::DOOR_LAYER;
}

/**
 * @brief Renders the projectile body.
 * @param window Render target.
 */
void Projectile::render(sf::RenderWindow& window, bool texturedMode)
{
	if (texturedMode && p_sprite.isLoaded())
	{
		p_sprite.setPosition(p_body.getPosition());
		if (p_direction != sf::Vector2f(0.f, 0.f))
			p_sprite.setRotation(sf::radians(std::atan2(p_direction.y, p_direction.x)));
		p_sprite.draw(window);
	}
	else
	{
		window.draw(p_body);
	}
}

/**
 * @brief Returns the projectile collision bounds.
 */
sf::FloatRect Projectile::getCollisionBounds() const
{
	return p_body.getGlobalBounds();
}

/**
 * @brief Returns the projectile collision filter profile.
 */
CollisionProfile Projectile::getCollisionProfile() const
{
	return p_collisionProfile;
}

/**
 * @brief Indicates whether the projectile should be removed from the world.
 */
bool Projectile::shouldDestroy() const
{
	return p_shouldDestroy;
}

/**
 * @brief Returns the damage amount this projectile applies on hit.
 */
int Projectile::applyDamage() const
{
	return p_damage;
}

/**
 * @brief Initializes the projectile shape.
 */
void Projectile::init_body()
{
	p_body.setSize(sf::Vector2f(10.f, 10.f));
	p_body.setFillColor(sf::Color::White);
	p_body.setOrigin(p_body.getSize() / 2.f);
	p_body.setPosition(p_spawnPoint);
}

bool Projectile::configureSprite(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize)
{
	if (!p_sprite.configure(texturePath, animations, tileCutoutSize))
		return false;

	const sf::FloatRect spriteBounds = p_sprite.getGlobalBounds();
	if (spriteBounds.size.x <= 0.f || spriteBounds.size.y <= 0.f)
		return false;

	p_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
	p_sprite.setScale(sf::Vector2f(
		p_body.getSize().x / spriteBounds.size.x,
		p_body.getSize().y / spriteBounds.size.y));
	p_sprite.setPosition(p_body.getPosition());

	return true;
}

bool Projectile::configureSpriteRects(const std::string& texturePath, const SpriteAnimationRectMap& animations)
{
	if (!p_sprite.configureWithRects(texturePath, animations))
		return false;

	const sf::FloatRect spriteBounds = p_sprite.getGlobalBounds();
	if (spriteBounds.size.x <= 0.f || spriteBounds.size.y <= 0.f)
		return false;

	p_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
	p_sprite.setScale(sf::Vector2f(
		p_body.getSize().x / spriteBounds.size.x,
		p_body.getSize().y / spriteBounds.size.y));
	p_sprite.setPosition(p_body.getPosition());

	return true;
}

/**
 * @brief Decrements lifetime and destroys the projectile when expired.
 * @param dt Delta time in seconds.
 */
void Projectile::onExpire(float dt)
{
	p_lifetime -= dt;
	if (p_lifetime <= 0.f)
		destroy();
}

/**
 * @brief Marks the projectile for destruction.
 */
void Projectile::destroy()
{
	p_shouldDestroy = true;
}

