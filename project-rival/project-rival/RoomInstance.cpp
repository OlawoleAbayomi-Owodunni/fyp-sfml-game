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
			// Walls
			if (tile == Tile::WALL) {
				// setup wall shape
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
			// Doors
			else if (tile == Tile::DOOR) {
				// setup door shape
				sf::RectangleShape door;

				// determine direction to place door
				DoorDirection dir;
				if (row == 0) dir = DoorDirection::NORTH;
				else if (row == plan.height - 1) dir = DoorDirection::SOUTH;
				else if (col == 0) dir = DoorDirection::WEST;
				else if (col == plan.width - 1) dir = DoorDirection::EAST;
				if (dir == DoorDirection::NORTH || dir == DoorDirection::SOUTH)
					door.setSize(sf::Vector2f(tileSize, tileSize / 2.f));
				else
					door.setSize(sf::Vector2f(tileSize / 2.f, tileSize));

				door.setOrigin(door.getSize() / 2.f);
				door.setPosition(worldPos + sf::Vector2f(col * tileSize, row * tileSize));
				door.setFillColor(sf::Color(150, 75, 0)); // brown doors

				ri_staticShapes.push_back(door);

				//setup collider for door
				StaticCollision collider(door.getGlobalBounds(), CollisionLayer::DOOR_LAYER,
					CollisionLayer::PLAYER_LAYER | CollisionLayer::ENEMY_LAYER | CollisionLayer::PLAYER_BULLET_LAYER | CollisionLayer::ENEMY_BULLET_LAYER);

				ri_staticColliders.push_back(collider);
			}
		}
	}
	// initialise spawners (DEBUG)
	for (auto& spawner : plan.spawners) {
		// setup spawn point shape
		sf::RectangleShape spawnPoint;
		spawnPoint.setSize(sf::Vector2f(tileSize, tileSize) / 8.f);
		spawnPoint.setOrigin(spawnPoint.getSize() / 2.f);
		spawnPoint.setPosition(worldPos + static_cast<sf::Vector2f>(spawner.tilePos) * tileSize);

		if (spawner.type == SpawnerType::PlayerSpawner) {
			spawnPoint.setFillColor(sf::Color::Green);
		}
		else if (spawner.type == SpawnerType::EnemySpawner) {
			spawnPoint.setFillColor(sf::Color::Red);
		}
		else if (spawner.type == SpawnerType::PortalSpawner) {
			spawnPoint.setFillColor(sf::Color::Blue);
		}

		ri_staticShapes.push_back(spawnPoint);
	}
	// initialise triggers (DEBUG)
	for (auto& trigger : plan.triggers) {
		// setup trigger shape
		sf::RectangleShape triggerShape;
		sf::Vector2f positionOffset = sf::Vector2f(0.f, 0.f);
		if (trigger.type == TriggerType::PortalTrigger) 
		{
			triggerShape.setSize(sf::Vector2f(tileSize, tileSize) * 3.f); // 3x the size of a tile
			triggerShape.setFillColor(sf::Color(0, 0, 255, 100)); // semi-transparent blue for portal trigger
		}
		else if (trigger.type == TriggerType::DoorTrigger)
		{
			DoorDirection dir;
			if (trigger.tilePos.y == 0) dir = DoorDirection::NORTH;
			else if (trigger.tilePos.y == plan.height - 1) dir = DoorDirection::SOUTH;
			else if (trigger.tilePos.x == 0) dir = DoorDirection::WEST;
			else if (trigger.tilePos.x == plan.width - 1) dir = DoorDirection::EAST;

			if (dir == DoorDirection::NORTH || dir == DoorDirection::SOUTH)
				triggerShape.setSize(sf::Vector2f(tileSize, tileSize / 4.f));
			else
				triggerShape.setSize(sf::Vector2f(tileSize / 4.f, tileSize));

			switch (dir)
			{
			case DoorDirection::NORTH:
				positionOffset = sf::Vector2f(0.f, tileSize * 1.7f);
				break;
			case DoorDirection::SOUTH:
				positionOffset = sf::Vector2f(0.f, -tileSize * 1.7f);
				break;
			case DoorDirection::WEST:
				positionOffset = sf::Vector2f(tileSize * 1.3f, 0.f);
				break;
			case DoorDirection::EAST:
				positionOffset = sf::Vector2f(-tileSize * 1.3f, 0.f);
				break;
			}

			triggerShape.setFillColor(sf::Color(150, 75, 0, 100)); // semi-transparent brown for door trigger
		}
		triggerShape.setOrigin(triggerShape.getSize() / 2.f);
		triggerShape.setPosition(worldPos + static_cast<sf::Vector2f>(trigger.tilePos) * tileSize + positionOffset);

		ri_staticShapes.push_back(triggerShape);

		// setup collider for trigger
		StaticCollision collider;
		if (trigger.type == TriggerType::PortalTrigger) {
			collider = StaticCollision(triggerShape.getGlobalBounds(), CollisionLayer::PORTAL_TRIGGER_LAYER,
				CollisionLayer::PLAYER_LAYER);
		}
		else if (trigger.type == TriggerType::DoorTrigger) {
			collider = StaticCollision(triggerShape.getGlobalBounds(), CollisionLayer::DOOR_TRIGGER_LAYER,
				CollisionLayer::PLAYER_LAYER);
		}
		
		ri_staticColliders.push_back(collider);
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
