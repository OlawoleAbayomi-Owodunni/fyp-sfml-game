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

## Combat / projectiles

Core files:

- `project-rival/Projectile.h/.cpp`
- `project-rival/NormalBulletProjectile.h/.cpp`

Key behaviours:

- Projectiles move each update and can expire or be destroyed on collision.
- Damage is provided via `applyDamage()`.

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

Extension points:

- Introduce enemy attacks (turrets firing, cooldowns).
- Add navigation constraints within rooms.

## LLM dialogue (planned)

Core idea:

- A dialogue subsystem that can send a “conversation context” to a local LLM and receive responses.

Extension points:

- Prompt templates, safety constraints, and tool-like structured outputs.
- Caching and performance considerations.
