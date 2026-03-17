#pragma once
#include "SFML/Graphics.hpp"
class Projectile
{
public:
	Projectile(sf::Vector2f spawnPoint);
	~Projectile() = default;

	virtual void init() = 0;
	virtual void update(double dt) = 0;
	virtual void render(sf::RenderWindow& window);

protected:
	void init_body();

	sf::RectangleShape p_body;
	sf::Vector2f p_spawnPoint;
	sf::Vector2f p_target;
};

