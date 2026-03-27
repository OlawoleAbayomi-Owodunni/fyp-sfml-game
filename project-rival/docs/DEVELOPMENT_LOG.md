# Development log (weekly / milestone)

This document tracks technical progress over time in a supervisor-friendly way.

## How to use this log ---->DELETE WHEN PROJECT IS COMPLETE<-----

- Add a new section each **week** (recommended) or per **milestone**.
- Each entry should be short:
  - **Summary**: 2–5 bullets
  - **Technical notes** (optional): 2–6 bullets
  - **Evidence**: commit hashes
  - **Why / intent**: add when available

---

## Week of 2026-03-25

### Summary
- Implemented early room/level generation for a combat room (variable size, obstacles, enemy spawn markers).
- Added static wall colliders and integrated wall collision checks for player, enemies, and bullets.
- Updated architecture docs to describe the new rooms pipeline and collision additions.

### Technical notes
- Added `RoomPlan` + `IRoomGenerator` contract in `RoomBlueprint.h`.
- Implemented `CombatRoom` generator (outer walls, random interior obstacles, random enemy spawner tiles).
- Added `RoomInstance` + `StaticCollision` to build renderable wall shapes and colliders from a `RoomPlan`.
- Extended collision layers with `WALL_LAYER` and wired masks for entities/projectiles.
- Updated `Game` to generate an active room, render it, spawn enemies at room spawn points, and handle basic wall collision rollback.

### Evidence (commits)
- `f8e0d87` — FEATURE(Levels): Generate combat rooms with wall colliders and enemy spawns

---

## Week of 2026-03-27

### Summary
- Added multiple room types (spawn room and portal room) alongside combat rooms.
- Added doors, locked-door collision, and trigger volumes for doors and portals.
- Implemented an early combat wave loop: entering a combat door trigger starts waves, locks doors, and clears/unlocks when complete.

### Technical notes
- Extended `RoomPlan` in `RoomBlueprint.h` to include `doors`, `triggers`, and an `isCleared` state flag.
- Updated `RoomInstance` to build:
  - wall colliders (`WALL_LAYER`)
  - door colliders (`DOOR_LAYER`) only when doors are locked
  - trigger colliders (`DOOR_TRIGGER_LAYER`, `PORTAL_TRIGGER_LAYER`) for gameplay interactions
- Updated collision response to use per-entity stored previous positions (`Player::hitWall()`, `Enemy::hitWall()`).
- Updated `Game` to manage a small room list, spawn player/enemies from spawners, and handle combat waves.

### Evidence (commits)
- `4f9cdbb` — FEATURE(Levels): Add Spawn/Portal rooms with triggers and player spawn placement
- `1d22e64` — Doors generated and visually represented
- `77c0739` — Door triggers added and collision with the player
- `4f44527` — Wave Spawner logic added
- `fd1bb72` — Wave System implemented

## Week of 2026-03-19

### Summary
- Introduced a minimal collision system using layers + masks.
- Wired projectile-vs-enemy and player-vs-enemy collisions into the game loop with basic damage/death handling.

### Technical notes
- Added `ICollidable`, `CollisionLayer` bitflags, `CollisionProfile`, and `CollisionCheck::areColliding(...)`.
- Integrated collision profiles into `Player`, `Enemy`, and `Projectile`.
- Added basic health/damage plumbing (`takeDamage`, `isDead`) and bullet destruction on hit.

### Evidence (commits)
- `ec7a87f` — FEATURE(Collisions): Added simple layer/mask collisions

---

## Week of 2026-03-17

### Summary
- Added aiming (mouse or right stick) and a visible reticle for the player.
- Added a basic projectile system so the player can shoot and bullets expire/clean up.

### Technical notes
- Added `Projectile` base class and `NormalBulletProjectile` implementation.
- Updated `Game::update(dt)` to pass mouse world coordinates into `Player::update(dt, mousePos)`.
- Updated architecture docs to document aiming + projectile ownership.

### Evidence (commits)
- `5b67edc` — FEATURE (Aiming): Added player aiming reticle and Projectile shooting prototype

---

## Week of 2026-03-16

### Summary
- Continued enemy system iteration and documentation updates.
- Added early art/source sprite assets.

### Technical notes
- Updated architecture docs and committed generated merge summary artifacts.
- Added sprite source file(s) under `ASSETS/SPRITES/`.

### Evidence (commits)
- `4c0f9e5` — DOC UPDATE
- `0b9219b` — Sprite files added

---

## Week of 2026-03-15

### Summary
- Implemented steering-based enemy movement and introduced multiple enemy types.
- Refactored game ownership to manage enemies via `std::vector<std::unique_ptr<Enemy>>`.

### Technical notes
- Added steering behaviours (seek/arrive) and a `SteeringAgent`.
- Refactored `Enemy` into an abstract base and implemented `GruntEnemy` and `TurretEnemy`.
- Updated `Game` to spawn/update/render enemies in loops and to set the player as the target.
- Added high-level architecture documentation and automation helpers (staged/merge summaries).

### Evidence (commits)
- `843a16a` — FEATURE(enemies): steering behaviours, enemy types, and multi-enemy spawning

---

## Week of 2026-01-22

### Summary
- Added Windows controller (XInput) support and integrated it into player controls.
- Improved player movement so it supports omni-directional input (keyboard and controller stick).

### Technical notes
- Introduced `Gamepad` abstraction to read sticks/triggers/buttons and apply deadzones.
- Added `InputManager` as a simple static access point for reading controller state.
- Updated `player` to read controller input and (optionally) trigger rumble.

### Evidence (commits)
- `a1be98e` — Controller Support
- `f2663af` — Player: Omni Directional movement

---

## Week of 2026-01-21

### Summary
- Wired the `player` entity into the game so it updates and renders during gameplay.
- Implemented/iterated on basic player movement.
- Synced work with `main` via merge.

### Technical notes
- `Game` now owns a `player` instance and calls its `update(dt)` and `render(...)`.
- Movement logic was iterated in `player.cpp`.

### Evidence (commits)
- `cb409c7` — Player: called in game
- `c324a0b` — Player class: movement
- `1f10f8e` — Player class: movement
- `1aecc43` — merge branch 'main'

---

## Week of 2025-11-24 (project setup)

### Summary
- Initial SFML project setup, solution/project files, and base game scaffolding.
- Added basic assets (font/logo) and a `.gitignore`.

### Evidence (commit)
- `ca2c920` — Setup + `.gitignore`

---
