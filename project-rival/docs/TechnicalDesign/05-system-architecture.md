# 5) High-level system architecture

[Back to Technical Design](README.md)

This chapter summarizes the major subsystems, who owns what, and how data flows.

> Note: The project already has detailed architecture docs. This chapter consolidates them into one place.

## Source of truth (existing architecture docs)

- `../Architecture/README.md` (index)

## Major subsystems

### Game loop / orchestration

- `Game` owns the top-level SFML window and drives update + render.
- `Game` is the best place for “systems” that require access to multiple owners (e.g., player bullets vs enemies).
- `Game` manages the current floor state (room plans, room instances, corridor instances) and handles spawning entities from spawner tiles.

### Input

- `InputManager` provides a simple access point.
- `Gamepad` wraps XInput and normalizes inputs.

### Player

- `Player` updates movement + aiming + projectiles.
- Player currently owns projectiles (prototype choice).

### Enemies / AI

- `Enemy` base class + concrete types (e.g., `GruntEnemy`, `TurretEnemy`).
- Steering behaviours are used for motion (seek/arrive).

### Combat / projectiles

- `Projectile` base class + concrete projectile types.
- Damage is applied on collision; bullets can be destroyed on hit.

### Collision

- Layer/mask filtering using `CollisionLayer` + `CollisionProfile`.
- AABB intersection checks via `CollisionCheck::areColliding(...)`.
- Collision response is currently handled at a high level (rollback for player/enemies; destroy bullets on wall hit).

### Rooms / generation

- `RoomPlan` contains tile/spawn/door/trigger data.
- `IRoomGenerator` builds a `RoomPlan` (e.g., `CombatRoom`).
- `RoomInstance` builds runtime shapes + `StaticCollision` colliders.

### Floor generation / layout (prototype)

- `FloorPlan` is a small graph (nodes=rooms, edges=connections).
- `FloorGenerator` builds a deterministic floor plan from a seed.
- `FloorLayoutGenerator` embeds rooms into a simple 2D grid.
- `RoomDoorUtils` applies connectivity by clearing/adding doors on room plans.

Corridors are generated in `Game` from floor edges and appended as extra `RoomInstance` objects.

### LLM dialogue (prototype integration)

- `LLMService` provides a small wrapper around the vendored `LLMWrapper` implementation.
- The wrapper is located under `external/fyp-llm-lib` and is intended to be used for NPC dialogue.

## Ownership + data flow (prototype)

- `Game` owns:
  - `Player`
  - enemies list (`std::vector<std::unique_ptr<Enemy>>`)
  - room plans (`std::vector<RoomPlan>`) and room instances (`std::vector<RoomInstance>`)
  - a floor plan + a simple floor layout (prototype)
- `Player` owns:
  - projectile list (`std::vector<std::unique_ptr<Projectile>>`)

Data flow example:

- `Game` updates player and enemies.
- `Game` checks collisions (player/enemy, bullets/enemy, entities vs walls/doors, triggers).
- `Game` renders rooms → player → enemies.

In the current prototype, the floor is rendered as multiple rooms simultaneously and the camera follows the player.

Game management input:

- Exit: `Escape` or controller `Start`
- Restart: `R` or controller `Select`
