#include "Enemy.h"
using namespace sf;

/**
 * @file Enemy.cpp
 * @brief Implements shared enemy state such as health, rendering, and collision profile.
 */

/**
 * @brief Constructs an enemy with a start position and maximum health.
 *
 * Also initializes the collision profile for the base enemy.
 *
 * @param startPos World position used for initial placement and resets.
 * @param totalHealth Maximum (and initial) health.
 */
Enemy::Enemy(const sf::Vector2f startPos, int totalHealth) :
	e_startPos(startPos), e_maxHealth(totalHealth)
{
	e_health = e_maxHealth;
	e_isDead = false;

	e_collisionProfile.layer = CollisionLayer::ENEMY_LAYER;
	e_collisionProfile.mask = CollisionLayer::PLAYER_LAYER | CollisionLayer::PLAYER_BULLET_LAYER | CollisionLayer::WALL_LAYER | CollisionLayer::DOOR_LAYER | CollisionLayer::DAMAGE_TRIGGER_LAYER;
}

/**
 * @brief Initializes the visual body shape.
 * @param size Rectangle size.
 * @param colour Fill colour.
 */
void Enemy::initBody(const sf::Vector2f& size, const sf::Color& colour)
{
	//--------- body setup ---------//
	e_body.setSize(size);
	e_body.setFillColor(colour);
	e_body.setOrigin(size / 2.f);
	e_body.setPosition(e_startPos);
	syncSpriteToBody();
}

/**
 * @brief Renders the enemy body.
 * @param window Render target.
 */
void Enemy::render(sf::RenderWindow& window, bool texturedMode)
{
	syncSpriteToBody();

	if (texturedMode && e_sprite.isLoaded())
		e_sprite.draw(window);
	else
		window.draw(e_body);
}

/**
 * @brief Resets runtime state back to the spawn position with full health.
 */
void Enemy::reset()
{
	e_body.setPosition(e_startPos);
	e_health = e_maxHealth;
	e_isDead = false;
	syncSpriteToBody();
}

bool Enemy::configureSprite(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize)
{
	if (!e_sprite.configure(texturePath, animations, tileCutoutSize))
		return false;

	const sf::FloatRect spriteBounds = e_sprite.getGlobalBounds();
	if (spriteBounds.size.x <= 0.f || spriteBounds.size.y <= 0.f)
		return false;

	e_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
	e_sprite.setScale(sf::Vector2f(
		e_body.getSize().x / spriteBounds.size.x,
		e_body.getSize().y / spriteBounds.size.y));
	e_sprite.setColor(e_body.getFillColor());
	e_sprite.setPosition(e_body.getPosition());

	return true;
}

bool Enemy::configureSpriteRects(const std::string& texturePath, const SpriteAnimationRectMap& animations)
{
	if (!e_sprite.configureWithRects(texturePath, animations))
		return false;

	const sf::FloatRect spriteBounds = e_sprite.getGlobalBounds();
	if (spriteBounds.size.x <= 0.f || spriteBounds.size.y <= 0.f)
		return false;

	e_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
	e_sprite.setScale(sf::Vector2f(
		e_body.getSize().x / spriteBounds.size.x,
		e_body.getSize().y / spriteBounds.size.y));
	e_sprite.setColor(e_body.getFillColor());
	e_sprite.setPosition(e_body.getPosition());

	return true;
}

void Enemy::syncSpriteToBody()
{
	if (!e_sprite.isLoaded())
		return;

	e_sprite.setPosition(e_body.getPosition());
}

/**
 * @brief Returns the collision bounds for this enemy.
 * @return Global bounds of the enemy body shape.
 */
sf::FloatRect Enemy::getCollisionBounds() const
{
	return e_body.getGlobalBounds();
}

/**
 * @brief Returns the collision profile used for collision filtering.
 * @return Collision layer and mask.
 */
CollisionProfile Enemy::getCollisionProfile() const
{
	return e_collisionProfile;
}

/**
 * @brief Applies damage and triggers death when health reaches zero.
 * @param damage Damage amount to subtract.
 */
void Enemy::takeDamage(int damage)
{
	e_health -= damage;
	if (e_health <= 0)
		onDeath();
}

/**
 * @brief Marks this enemy as dead.
 */
void Enemy::onDeath()
{
	e_isDead = true;
}

/**
 * @brief Indicates whether this enemy has died.
 */
bool Enemy::isDead() const
{
	return e_isDead;
}
