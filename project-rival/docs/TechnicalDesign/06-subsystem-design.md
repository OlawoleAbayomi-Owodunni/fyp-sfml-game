# 6) Detailed subsystem design

[Back to Technical Design](README.md)

This chapter describes each subsystem’s core classes/files, key data, and where we expect change.

## Collision

Core files:

- `project-rival/Collision.h`, `project-rival/Collision.cpp`

Key concepts:

- `CollisionLayer` bitflags (e.g., player, enemy, wall).
- `CollisionProfile` (layer + mask).
- `ICollidable` (bounds + profile).

Current behaviour:

- Collision checks are centralized in `Game::update(dt)` (via `Game::CollisionChecks()`) using `CollisionCheck::areColliding(...)`.
- Response is currently a simple rollback for player/enemies (`hitWall()`) and destruction for projectiles on wall/door hit.

Extension points:

- Replace basic “rollback” collision response with axis-separated slide.
- Add broadphase if entity counts become high.

## Rooms / procedural generation

Core files:

- `project-rival/RoomBlueprint.h` (RoomPlan + `IRoomGenerator`)
- `project-rival/CombatRoom.h/.cpp` (prototype generator)
- `project-rival/RoomInstance.h/.cpp` (runtime build)
- `project-rival/StaticCollision.h/.cpp` (wall collider type)

Key data structures:

- `RoomPlan.tileMap`: 1D array with indexing `row * width + col`.
- `RoomPlan.spawners`: tile-space markers that can be converted to world-space.

Extension points:

- Add additional generators per room type.
- Add doors and room-to-room connections.
- Seeded deterministic dungeon/floor graph generation.

Current behaviour:

- `CombatRoom` currently chooses a random width/height in a small range.
- Spawn points and obstacle density are derived from interior size (via `interiorArea`).
- `RoomInstance` converts all `Tile::WALL` tiles (outer walls + obstacles) into wall shapes + `StaticCollision` colliders.
- `Game::generateRoom(roomId)` rebuilds the `RoomInstance` from the current `RoomPlan` (used after door lock/unlock or wave generation).
- `Game::spawnEnemies(roomId)` spawns runtime enemies from `RoomPlan.spawners` (world position = `roomWorldPos + tilePos * tileSize`).

Doors and triggers (current):

- Door tiles can span 2–3 tiles depending on room size.
- Door tiles create `DoorTrigger` entries.
- Locked doors become `DOOR_LAYER` colliders at build time.
- Portal rooms can add a `PortalTrigger` collider.

Corridors (current):

- Corridors are generated at runtime in `Game::buildCorridors()` as `RoomPlan` objects of type `RoomType::CORRIDOR`.
- Corridor plans are immediately built into `RoomInstance` objects and appended to `m_roomInstances`.
- Corridors use `Tile::WALL` for borders and a strip of `Tile::FLOOR` through the middle.

## Floor generation / layout (prototype)

Core files:

- `project-rival/FloorBlueprint.h` (`FloorPlan`)
- `project-rival/FloorGenerator.h/.cpp`
- `project-rival/FloorLayout.h/.cpp` (`FloorLayoutGenerator`)
- `project-rival/RoomDoorUtils.h/.cpp`

Key behaviours:

- The floor plan is generated deterministically from `dungeonSeed + floorId`.
- Rooms are represented as a graph (edges = connections).
- The current generator enforces simple constraints (spawn first, portal last, no direct spawn -> portal edge).
- Room count is currently randomized per floor in the range `[3..10]`.
- Middle rooms currently resolve to `COMBAT` (other room types are planned).
- A simple BFS-based layout places rooms on a 2D grid without overlaps.
- Doors are cleared and re-added to room plans based on graph edges.

Current runtime build:

- `Game::buildFloorInstance()` builds all room instances from their plans.
- `Game::buildCorridors()` builds additional corridor instances that connect matching doors between connected rooms.

## Dungeon progression (prototype)

Core files:

- `project-rival/DungeonPlan.h`

Key behaviours:

- `DungeonPlan` tracks a dungeon seed, current floor id, total floor count, and whether the dungeon is complete.
- Portal triggers can request a floor advance; `Game` consumes the request and calls `loadNewFloor()`.

## Combat / projectiles

Core files:

- `project-rival/Projectile.h/.cpp`
- `project-rival/NormalBulletProjectile.h/.cpp`

Key behaviours:

- Projectiles move each update and can expire or be destroyed on collision.
- Damage is provided via `applyDamage()`.

Current behaviour:

- Player bullets are destroyed when they hit walls.
- `NormalBulletProjectile` currently applies 10 damage.

(Projectiles also collide with locked doors.)

Extension points:

- Move projectile ownership from `Player` into a weapon system or a world-level projectile manager.
- Add weapon types (melee, missiles, etc.).

## Enemies / AI (steering)

Core files:

- `project-rival/Enemy.h/.cpp`
- `project-rival/GruntEnemy.h/.cpp`
- `project-rival/SeekBehaviour.*`, `project-rival/ArriveBehaviour.*`

Key algorithms:

- Steering-based movement (seek/arrive) with behaviour switching based on distance.

Current behaviour:

- Enemies track max health (`e_maxHealth`) and can be reset to their start state via `Enemy::reset()`.
- Enemies respond to wall/door collisions via `hitWall()` (e.g., `GruntEnemy` rolls back).

Extension points:

- Introduce enemy attacks (turrets firing, cooldowns).
- Add navigation constraints within rooms.

## LLM integration (prototype)

Core files:

- `project-rival/LLMService.h/.cpp`
- `external/fyp-llm-lib/llm/LLMWrapper.h` (vendored dependency)

Current behaviour:

- `LLMService::init(modelPath)` loads a `.gguf` model via `LLMWrapper::LoadModel(...)`.
- `LLMService::generateResponse(prompt)` returns `LLMWrapper::Generate(prompt)` when ready; otherwise returns a simple error string.

Extension points:

- Provide a structured NPC dialogue API (speaker id, conversation history, system prompt).
- Add caching and streaming generation support (if supported by the wrapper).
- Define safety constraints and prompt templates.
