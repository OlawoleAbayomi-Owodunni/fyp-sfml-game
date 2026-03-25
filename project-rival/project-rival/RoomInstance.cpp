#include "RoomInstance.h"

void RoomInstance::buildFromPlan(const RoomPlan& plan, const sf::Vector2f& worldPos)
{
	ri_staticColliders.clear();
	ri_staticShapes.clear();

	if (!plan.isValid())
		return;

	const float tileSize = plan.tileSize;
	
	// initialise each tile
	for (int row = 0; row < plan.height; row++) {
		for (int col = 0; col < plan.width; col++) {
			const Tile tile = plan.getTile(row, col);
			if (tile == Tile::WALL) {
			// setup wall
			sf::RectangleShape wall;
			wall.setSize(sf::Vector2f(tileSize, tileSize));
			wall.setOrigin(wall.getSize() / 2.f);
			wall.setPosition(worldPos + sf::Vector2f(col * tileSize, row * tileSize));
			wall.setFillColor(sf::Color(100, 100, 100)); // grey walls

			ri_staticShapes.push_back(wall);

			//setup collider for wall
			StaticCollision collider(wall.getGlobalBounds(), CollisionLayer::WALL_LAYER,
				CollisionLayer::PLAYER_LAYER | CollisionLayer::ENEMY_LAYER | CollisionLayer::PLAYER_BULLET_LAYER | CollisionLayer::ENEMY_BULLET_LAYER);

			ri_staticColliders.push_back(collider);
		}
		}
	}
	// initialise spawners (DEBUG)
	for (auto& spawner : plan.spawners) {
		sf::RectangleShape spawnPoint;
		spawnPoint.setSize(sf::Vector2f(tileSize, tileSize) / 8.f);
		spawnPoint.setOrigin(spawnPoint.getSize() / 2.f);
		spawnPoint.setPosition(worldPos + static_cast<sf::Vector2f>(spawner.tilePos) * tileSize);
		if (spawner.type == SpawnerType::PlayerSpawner) {
			spawnPoint.setFillColor(sf::Color(0, 255, 0)); // green for player spawn
		}
		else if (spawner.type == SpawnerType::EnemySpawner) {
			spawnPoint.setFillColor(sf::Color(255, 0, 0)); // red for enemy spawn
		}

		ri_staticShapes.push_back(spawnPoint);
	}
}

const std::vector<StaticCollision>& RoomInstance::getStaticCollisions()
{
	return ri_staticColliders;
}

void RoomInstance::render(sf::RenderWindow& window)
{
	for (auto& shape : ri_staticShapes)
	{
		window.draw(shape);
	}
}

void RoomInstance::reset()
{
	ri_staticColliders.clear();
	ri_staticShapes.clear();
}
