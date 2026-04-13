#pragma once
#include<iostream>
#include <SFML/graphics.hpp>

#include "SeekBehaviour.h"
#include "ArriveBehaviour.h"

#include "Collision.h"

/**
 * @file Enemy.h
 * @brief Declares the enemy base class and shared enemy types.
 */

/**
 * @brief Concrete enemy archetypes.
 */
enum EnemyType
{
	GRUNT,
	TURRET,

	ENEMY_COUNT
};

class Enemy : public ICollidable
{
public:
	Enemy(const sf::Vector2f startPos, int totalHealth);

	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void render(sf::RenderWindow& window);
	virtual void reset();

	virtual void setTarget(const Vector2f& target) = 0;
	virtual EnemyType getEnemyType() const = 0;

	virtual sf::FloatRect getCollisionBounds() const override;
	virtual CollisionProfile getCollisionProfile() const override;
	virtual sf::Vector2f getPosition() const { return e_body.getPosition(); }

	virtual void hitWall() = 0;

	virtual void takeDamage(int damage);
	virtual void onDeath();
	virtual bool isDead() const;

protected:
	void initBody(const sf::Vector2f& size, const sf::Color& colour);

	sf::RectangleShape e_body;
	sf::Vector2f e_startPos;
	sf::Vector2f e_prevPos;
	sf::Vector2f e_target{ 0.f, 0.f };

	CollisionProfile e_collisionProfile;

	// entity stats
	int e_maxHealth;
	int e_health;
	bool e_isDead;
};

