# 5) High-level system architecture

[Back to Technical Design](README.md)

This chapter summarizes the major subsystems, who owns what, and how data flows.

> Note: The project already has detailed architecture docs. This chapter consolidates them into one place.

## Source of truth (existing architecture docs)

- `../Architecture/README.md` (index)

## Major subsystems

### Game loop / orchestration

- `Game` owns the top-level SFML window and drives update + render.
- `Game` is the integration point for cross-owner systems (collisions, room triggers, combat waves, floor transitions).
- `Game` manages floor runtime state (room plans, room instances, corridor instances) and handles spawning entities from spawner tiles.

### Input

- `InputManager` provides a simple access point.
- `Gamepad` wraps XInput and normalizes inputs.

### Player

- `Player` updates movement + aiming + weapon selection.
- `Player` emits projectile/melee trigger requests into game-owned runtime containers.

### Enemies / AI

- `Enemy` base class + concrete types (`GruntEnemy`, `TurretEnemy`).
- Steering behaviours are used for grunt movement (`seek`/`arrive`).

### Combat / projectiles / damage triggers

- `Projectile` base class + concrete projectile types.
- `DamageTrigger` supports short-lived melee hit volumes.
- Damage is applied on collision; bullets/triggers are destroyed/expired and removed by `Game`.

### Collision

- Layer/mask filtering using `CollisionLayer` + `CollisionProfile`.
- AABB intersection checks via `CollisionCheck::areColliding(...)`.
- Collision response is currently high-level rollback for player/enemies and projectile destruction on wall/locked-door hit.

### Rooms / generation

- `RoomPlan` contains tile/spawn/door/trigger data.
- `IRoomGenerator` builds room plans (`CombatRoom`, `SpawnRoom`, `PortalRoom`).
- `RoomInstance` builds runtime shapes + `StaticCollision` colliders.

### Floor generation / layout (prototype)

- `FloorPlan` is a small graph (nodes=rooms, edges=connections).
- `FloorGenerator` builds a deterministic floor plan from a seed.
- `FloorLayoutGenerator` embeds rooms into a simple 2D grid.
- `RoomDoorUtils` applies connectivity by clearing/adding doors on room plans.

Corridors are generated in `Game` from floor edges and appended as extra `RoomInstance` objects.

### LLM dialogue (prototype integration)

- `LLMService` wraps the vendored `LLMWrapper` (`external/fyp-llm-lib`).
- `Game` uses async init/request/poll APIs and currently exposes a debug generation trigger (`Num0`).

## Ownership + data flow (prototype)

- `Game` owns:
  - `Player`
  - enemies list (`std::vector<std::unique_ptr<Enemy>>`)
  - projectile list (`std::vector<std::unique_ptr<Projectile>>`)
  - active melee trigger list (`std::vector<std::unique_ptr<DamageTrigger>>`)
  - room plans + room instances
  - floor plan + floor layout
  - dungeon plan state
  - LLM service state

Data flow example:

- `Game` updates player/enemies and lifecycle-manages projectiles/triggers.
- `Game` checks collisions (entity hits, bullets, melee triggers, environment, portal/door triggers).
- `Game` processes combat waves/floor transitions/LLM polling.
- `Game` renders rooms → player → enemies → projectiles → triggers.

Current camera behavior supports both player-follow and floor-overview modes.

Game management input:

- Exit: `Escape` or controller `Start`
- Restart: `R` or controller `Select`
