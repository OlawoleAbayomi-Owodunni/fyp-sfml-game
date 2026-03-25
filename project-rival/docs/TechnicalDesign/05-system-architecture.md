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
- `Game` also owns the active room state and handles spawning enemies from room spawner tiles.

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

- `RoomPlan` contains tile/spawn data.
- `IRoomGenerator` builds a `RoomPlan` (e.g., `CombatRoom`).
- `RoomInstance` builds runtime shapes + `StaticCollision` colliders.

### LLM dialogue (planned)

- NPC dialogue system backed by a local LLM wrapper.
- Intended to be integrated as a separate module/library.

## Ownership + data flow (prototype)

- `Game` owns:
  - `Player`
  - enemies list (`std::vector<std::unique_ptr<Enemy>>`)
  - active room (`RoomPlan` + `RoomInstance`)
- `Player` owns:
  - projectile list (`std::vector<std::unique_ptr<Projectile>>`)

Data flow example:

- `Game` updates player and enemies.
- `Game` checks collisions (player/enemy, bullets/enemy, objects vs walls).
- `Game` renders room → player → enemies.

Game management input:

- Exit: `Escape` or controller `Start`
- Restart: `R` or controller `Select`
