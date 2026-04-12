# 1) Project structure

[Back to architecture index](README.md)

Most gameplay code is inside `project-rival/`.

## Suggested mental model (categories)

### Core / game loop

- `main.cpp`
- `Game.h` / `Game.cpp`

### Collision

- `Collision.h` / `Collision.cpp`

### Rooms / procedural generation

- `RoomBlueprint.h` (RoomPlan + tiles + doors + spawners + triggers + generator interface)
- `CombatRoom.h` / `CombatRoom.cpp` (combat room generator + wave spawning helpers)
- `SpawnRoom.h` / `SpawnRoom.cpp` (spawn room generator + player spawn marker)
- `PortalRoom.h` / `PortalRoom.cpp` (portal room generator + portal spawn + trigger)
- `RoomInstance.h` / `RoomInstance.cpp` (builds drawable shapes + static colliders from a `RoomPlan`)
- `StaticCollision.h` / `StaticCollision.cpp` (colliders for walls/doors/triggers)

### Floor generation / multi-room layout (prototype)

- `FloorBlueprint.h` (`FloorPlan`: room nodes + graph edges + adjacency list)
- `FloorGenerator.h` / `FloorGenerator.cpp` (generates a floor graph deterministically from a seed)
- `FloorLayout.h` / `FloorLayout.cpp` (embeds the floor graph into a simple 2D grid layout)
- `RoomDoorUtils.h` / `RoomDoorUtils.cpp` (adds/clears doors in a `RoomPlan` based on graph connectivity)

Corridors are currently generated inside `Game.cpp` (as temporary `RoomPlan`s converted into `RoomInstance`s).

### Dungeon / multi-floor run state (prototype)

- `DungeonPlan.h` (tracks dungeon seed, current floor id, floor count, and “dungeon complete” state)

### LLM integration (prototype)

- `LLMService.h` / `LLMService.cpp` (game-facing service wrapper)
- `external/fyp-llm-lib/llm/LLMWrapper.h` (vendored LLM wrapper)

### Entities

- `player.h` / `player.cpp`
- `Enemy.h` / `Enemy.cpp` (base class)
- `GruntEnemy.h` / `GruntEnemy.cpp` (moves using steering)
- `TurretEnemy.h` / `TurretEnemy.cpp` (stationary ranged attacker)

### Spawnables / loot (prototype)

- `Pickup.h` / `Pickup.cpp` (health/ammo/coin pickups)
- `Chest.h` / `Chest.cpp` (interactable chest trigger object)

### Quest system (prototype)

- `Quest.h` / `Quest.cpp` (`QuestData`, quest board generation, active quest tracking + run reward commit)

### Input

- `InputManager.h` / `InputManager.cpp`
- `Gamepad.h` / `Gamepad.cpp`

### Combat / projectiles (early prototype)

- `Projectile.h` / `Projectile.cpp` (base class)
- `NormalBulletProjectile.h` / `NormalBulletProjectile.cpp` (simple bullet)

### UI

- `PlayerHUD.h` / `PlayerHUD.cpp` (health/ammo/coins HUD overlay)
- `MenuUI.h` / `MenuUI.cpp` (main/pause/game-over/quest-board menu screens and actions)

### AI / Movement

- `Steering.h`

- `SeekBehaviour.h` / `SeekBehaviour.cpp`
- `ArriveBehaviour.h` / `ArriveBehaviour.cpp`

This category list is mainly to keep the codebase readable as the number of files grows.
