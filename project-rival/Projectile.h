#pragma once
#include "SFML/Graphics.hpp"

#include "Collision.h"
#include "CustomSprite.h"

class Projectile : public ICollidable
{
public:
	Projectile(sf::Vector2f spawnPoint, sf::Vector2f direction, float speed, int damage, bool isFromPlayer);
	virtual ~Projectile() = default;

	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void render(sf::RenderWindow& window, bool texturedMode);

	virtual sf::FloatRect getCollisionBounds() const override;
	virtual CollisionProfile getCollisionProfile() const override;

	virtual bool shouldDestroy() const;

	virtual int applyDamage() const;
	virtual void destroy();

protected:
	void init_body();
	virtual void onExpire(float dt);
	bool configureSprite(const std::string& texturePath, const SpriteAnimationMap& animations, const sf::Vector2i& tileCutoutSize);

	bool p_shouldDestroy{ false };

	sf::RectangleShape p_body;
	CustomSprite p_sprite;
	sf::Vector2f p_spawnPoint;
	sf::Vector2f p_direction;

	bool p_isFromPlayer;
	CollisionProfile p_collisionProfile;

	float p_lifetime{ 15.f };

	int p_damage;
	float p_speed;

};

