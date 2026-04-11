# 6) Detailed subsystem design

[Back to Technical Design](README.md)

This chapter describes each subsystem’s core classes/files, key data, and where we expect change.

## Collision

Core files:

- `project-rival/Collision.h`, `project-rival/Collision.cpp`

Key concepts:

- `CollisionLayer` bitflags (player/enemy/bullets/walls/doors/triggers).
- `CollisionProfile` (layer + mask).
- `ICollidable` (bounds + profile).

Current behaviour:

- Collision checks are centralized in `Game::CollisionChecks()` using `CollisionCheck::areColliding(...)`.
- Response is currently a simple rollback for player/enemies (`hitWall()`) and destruction for projectiles on wall/door hit.

## Rooms / procedural generation

Core files:

- `project-rival/RoomBlueprint.h` (`RoomPlan` + `IRoomGenerator`)
- `project-rival/CombatRoom.h/.cpp`, `project-rival/SpawnRoom.h/.cpp`, `project-rival/PortalRoom.h/.cpp`
- `project-rival/RoomInstance.h/.cpp` (runtime build)
- `project-rival/StaticCollision.h/.cpp` (static collider type)

Key data structures:

- `RoomPlan.tileMap`: 1D array with indexing `row * width + col`.
- `RoomPlan.spawners`: tile-space runtime spawn markers.
- `RoomPlan.doors` + `RoomPlan.triggers`: room connectivity/interaction metadata.

Current behaviour:

- `CombatRoom` chooses random width/height in a small range.
- Spawn points and obstacle density are derived from interior size (`interiorArea`).
- `RoomInstance` converts all `Tile::WALL` tiles into wall shapes + wall colliders.
- Door tiles are rendered; locked doors add `DOOR_LAYER` colliders.
- Trigger plans become portal/door trigger colliders.
- `Game::generateRoom(roomId)` rebuilds `RoomInstance` from current `RoomPlan`.
- `Game::spawnEnemies(roomId)` spawns enemies from `RoomPlan.spawners`.

Corridors (current):

- Generated at runtime in `Game::buildCorridors()` as temporary `RoomPlan` objects of type `RoomType::CORRIDOR`.
- Immediately built into `RoomInstance` objects and appended to `m_roomInstances`.

Hub-room reuse (current):

- `Game::buildHubWorld()` builds a single large room plan/instance (spawn + portal trigger) using the same room pipeline.

## Floor generation / layout (prototype)

Core files:

- `project-rival/FloorBlueprint.h` (`FloorPlan`)
- `project-rival/FloorGenerator.h/.cpp`
- `project-rival/FloorLayout.h/.cpp` (`FloorLayoutGenerator`)
- `project-rival/RoomDoorUtils.h/.cpp`

Key behaviours:

- Floor plan is generated deterministically from `dungeonSeed + floorId`.
- Rooms are a graph (edges = connections).
- Generator constraints: spawn first, portal last, no direct spawn -> portal edge.
- Room count currently randomized in `[3..10]`.
- Middle rooms currently resolve to `COMBAT`.
- BFS-style layout places rooms on a 2D grid without overlaps.
- Doors are cleared and re-added to room plans based on graph edges.

Runtime build:

- `Game::buildFloorInstance()` builds all room instances from room plans.
- `Game::buildCorridors()` builds and appends corridor instances connecting matched door spans.

## Dungeon progression (prototype)

Core files:

- `project-rival/DungeonPlan.h`

Key behaviours:

- Tracks dungeon seed, current floor id, floor count, completion flag.
- Portal trigger interaction requests floor advance; `Game` consumes request and loads next floor.
- Current completion behavior returns flow to hub mode.

## Player loadout/upgrades (prototype)

Core files:

- `project-rival/player.h/.cpp`
- `project-rival/Weapon.h/.cpp`
- `project-rival/PlayerWeapons.h/.cpp`

Current behaviour:

- Player keeps a loadout (`WeaponInLoadout`) with max 3 slots.
- Runtime weapon objects are rebuilt from loadout entries.
- Upgrade application currently adjusts health, speed, and ammo capacity.
- Weapon level currently scales gun damage/fire tuning.

## Combat / projectiles / melee triggers

Core files:

- `project-rival/Weapon.h/.cpp`
- `project-rival/PlayerWeapons.h/.cpp`
- `project-rival/EnemyWeapon.h/.cpp`
- `project-rival/Projectile.h/.cpp`
- `project-rival/NormalBulletProjectile.h/.cpp`
- `project-rival/DamageTrigger.h/.cpp`

Key behaviours:

- Weapons gate fire by cooldown and emit either projectiles or melee `DamageTrigger`s.
- Projectile damage is set by weapon spawn functions.
- `Game` owns projectile and active damage-trigger runtime vectors and updates/removes expired entries.
- Projectiles collide with walls and locked doors.

Extension points:

- Add richer fire modes/effects.
- Add new projectile and trigger types.

## Enemies / AI (steering)

Core files:

- `project-rival/Enemy.h/.cpp`
- `project-rival/GruntEnemy.h/.cpp`
- `project-rival/TurretEnemy.h/.cpp`
- `project-rival/SeekBehaviour.*`, `project-rival/ArriveBehaviour.*`

Current behaviour:

- `GruntEnemy` uses steering movement (`Seek`/`Arrive`) and melee attacks via `EnemyWeapon`.
- `TurretEnemy` aims at player target and fires projectiles on a randomized timer.
- Enemies use shared base health/death handling.

Extension points:

- Add navigation/path constraints.
- Add more enemy archetypes and attack patterns.

## LLM integration (prototype)

Core files:

- `project-rival/LLMService.h/.cpp`
- `external/fyp-llm-lib/llm/LLMWrapper.h` (vendored dependency)

Current behaviour:

- LLM is initialized asynchronously from `Game` via `LLMService::initAsync(modelPath)`.
- `Game::update(dt)` polls:
  - `tryConsumeInitResult()` for init completion/failure
  - `tryConsumeLatestResponse()` for completed generation output
- Generation requests are queued using `requestGenerate(prompt)` and gated by `isReady()` / `isBusy()`.
- `shutdown()` joins worker threads and unloads model.

Extension points:

- Structured NPC dialogue API (speaker id, context/history, templates).
- Streaming/caching/latency handling.
- Prompt safety/format constraints.
