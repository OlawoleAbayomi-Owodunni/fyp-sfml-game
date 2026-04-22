#include "RoomInstance.h"

/**
 * @file RoomInstance.cpp
 * @brief Builds and renders a concrete room instance from a `RoomPlan`.
 */

/**
 * @brief Builds render shapes and static colliders from a `RoomPlan`.
 *
 * Converts the plan tilemap into a set of SFML `RectangleShape`s (floors/walls/doors)
 * and a set of `StaticCollision` objects used for collision queries.
 *
 * @param plan Source plan to build from.
 * @param worldPos World-space origin for the room tilemap.
 */
void RoomInstance::buildFromPlan(const RoomPlan& plan, const sf::Vector2f& worldPos)
{
	ri_staticRoomColliders.clear();
	ri_staticRoomShapes.clear();
	ri_staticRoomSprites.clear();

	if (!plan.isValid())
		return;

	const float tileSize = plan.tileSize;
	const std::string levelAtlasPath = "ASSETS/SPRITES/Everything.png";
	const std::string teleporterAtlasPath = "ASSETS/SPRITES/LEVELS/Teleporter.png";

	auto addTileSprite = [&](const sf::Vector2f& position,
		const sf::Vector2f& shapeSize,
		const std::string& texturePath,
		const std::vector<sf::Vector2i>& frameTiles,
		const sf::Vector2i& frameSize,
		float fps = 1.f)
		{
			CustomSprite sprite;
			SpriteAnimationMap animations{
				{ "Idle", { frameTiles, fps, true } }
			};

			if (!sprite.configure(texturePath, animations, frameSize))
				return;

			const sf::FloatRect spriteBounds = sprite.getGlobalBounds();
			if (spriteBounds.size.x <= 0.f || spriteBounds.size.y <= 0.f)
				return;

			sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
			sprite.setScale(sf::Vector2f(
				shapeSize.x / spriteBounds.size.x,
				shapeSize.y / spriteBounds.size.y));
			sprite.setPosition(position);
			ri_staticRoomSprites.push_back(sprite);
		};
	
	// initialise each tile
	for (int row = 0; row < plan.height; row++) {
		for (int col = 0; col < plan.width; col++) {
			const Tile tile = plan.getTile(row, col);
			// Floors
			if (tile == Tile::FLOOR) {
				// setup floor shape
				sf::RectangleShape floor;
				floor.setSize(sf::Vector2f(tileSize, tileSize));
				floor.setOrigin(floor.getSize() / 2.f);
				floor.setPosition(worldPos + sf::Vector2f(col * tileSize, row * tileSize));
				floor.setFillColor(sf::Color(200, 200, 200)); // light grey floor
				ri_staticRoomShapes.push_back(floor);
				addTileSprite(floor.getPosition(), floor.getSize(), levelAtlasPath, { sf::Vector2i(72, 192) }, sf::Vector2i(64, 64));
			}

			// Walls
			if (tile == Tile::WALL) {
				// setup wall shape
				sf::RectangleShape wall;
				wall.setSize(sf::Vector2f(tileSize, tileSize));
				wall.setOrigin(wall.getSize() / 2.f);
				wall.setPosition(worldPos + sf::Vector2f(col * tileSize, row * tileSize));
				wall.setFillColor(sf::Color(100, 100, 100)); // grey walls

				ri_staticRoomShapes.push_back(wall);
				addTileSprite(wall.getPosition(), wall.getSize(), levelAtlasPath, { sf::Vector2i(256, 128) }, sf::Vector2i(64, 64));

				//setup collider for wall
				StaticCollision collider(wall.getGlobalBounds(), CollisionLayer::WALL_LAYER,
					CollisionLayer::PLAYER_LAYER | CollisionLayer::ENEMY_LAYER | CollisionLayer::PLAYER_BULLET_LAYER | CollisionLayer::ENEMY_BULLET_LAYER);

				ri_staticRoomColliders.push_back(collider);
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

				ri_staticRoomShapes.push_back(door);
				addTileSprite(door.getPosition(), door.getSize(), levelAtlasPath, { sf::Vector2i(576, 256) }, sf::Vector2i(64, 64));

				//setup collider for door if door is locked
				for (auto& doorObj : plan.doors) {
					sf::Vector2i distToStart =  sf::Vector2i(col, row) - doorObj.tileStartPos;
					distToStart = sf::Vector2i(std::abs(distToStart.x), std::abs(distToStart.y));
					if (doorObj.tileStartPos == (sf::Vector2i(col, row) - distToStart))
					{
						if (doorObj.isLocked) {
							StaticCollision collider(door.getGlobalBounds(), CollisionLayer::DOOR_LAYER,
								CollisionLayer::PLAYER_LAYER | CollisionLayer::ENEMY_LAYER | CollisionLayer::PLAYER_BULLET_LAYER | CollisionLayer::ENEMY_BULLET_LAYER);

							ri_staticRoomColliders.push_back(collider);
						}
						break;
					}
				}
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
			addTileSprite(spawnPoint.getPosition(), spawnPoint.getSize(), levelAtlasPath, { sf::Vector2i(72, 192) }, sf::Vector2i(64, 64));
		}
		else if (spawner.type == SpawnerType::EnemySpawner) {
			spawnPoint.setFillColor(sf::Color::Red);
			addTileSprite(spawnPoint.getPosition(), spawnPoint.getSize(), levelAtlasPath, { sf::Vector2i(256, 128) }, sf::Vector2i(64, 64));
		}
		else if (spawner.type == SpawnerType::PortalSpawner) {
			spawnPoint.setFillColor(sf::Color::Blue);
			addTileSprite(spawnPoint.getPosition(), spawnPoint.getSize(), teleporterAtlasPath,
				{ sf::Vector2i(2, 0), sf::Vector2i(3, 0), sf::Vector2i(2, 1), sf::Vector2i(3, 1) }, sf::Vector2i(144, 144), 6.f);
		}

		ri_staticRoomShapes.push_back(spawnPoint);
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

		ri_staticRoomShapes.push_back(triggerShape);
		if (trigger.type == TriggerType::PortalTrigger)
			addTileSprite(triggerShape.getPosition(), triggerShape.getSize(), teleporterAtlasPath,
				{ sf::Vector2i(2, 2), sf::Vector2i(3, 2), sf::Vector2i(2, 3), sf::Vector2i(3, 3) }, sf::Vector2i(144, 144), 8.f);
		else if (trigger.type == TriggerType::DoorTrigger)
			addTileSprite(triggerShape.getPosition(), triggerShape.getSize(), levelAtlasPath, { sf::Vector2i(576, 256) }, sf::Vector2i(64, 64));


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
		
		ri_staticRoomColliders.push_back(collider);
	}
}

/**
 * @brief Returns the static colliders built for this room instance.
 * @return Reference to the collider list.
 */
const std::vector<StaticCollision>& RoomInstance::getStaticRoomColliders()
{
	return ri_staticRoomColliders;
}

/**
 * @brief Renders all prebuilt static room shapes.
 * @param window Target render window.
 */
void RoomInstance::render(sf::RenderWindow& window, bool texturedMode)
{
	if (texturedMode && !ri_staticRoomSprites.empty() && ri_staticRoomSprites.size() == ri_staticRoomShapes.size())
	{
		for (auto& sprite : ri_staticRoomSprites)
			sprite.draw(window);
		return;
	}

	for (auto& shape : ri_staticRoomShapes)
	{
		window.draw(shape);
	}
}

/**
 * @brief Clears all built shapes and colliders.
 */
void RoomInstance::reset()
{
	ri_staticRoomColliders.clear();
	ri_staticRoomShapes.clear();
	ri_staticRoomSprites.clear();
}

void RoomInstance::addSpriteForShape(const sf::RectangleShape& shape, const std::string& texturePath, const sf::Vector2i& framePosition, const sf::Vector2i& frameSize)
{
	CustomSprite sprite;
	SpriteAnimationMap animations{
		{ "Idle", { std::vector<sf::Vector2i>{ framePosition }, 1.f, true } }
	};

	if (!sprite.configure(texturePath, animations, frameSize))
		return;

	const sf::FloatRect spriteBounds = sprite.getGlobalBounds();
	if (spriteBounds.size.x <= 0.f || spriteBounds.size.y <= 0.f)
		return;

	sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
	sprite.setScale(sf::Vector2f(
		shape.getSize().x / spriteBounds.size.x,
		shape.getSize().y / spriteBounds.size.y));
	sprite.setPosition(shape.getPosition());
	ri_staticRoomSprites.push_back(sprite);
}
