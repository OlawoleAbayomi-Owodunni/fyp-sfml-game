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

### Entities

- `player.h` / `player.cpp`
- `Enemy.h` / `Enemy.cpp` (base class)
- `GruntEnemy.h` / `GruntEnemy.cpp` (moves using steering)
- `TurretEnemy.h` / `TurretEnemy.cpp` (static enemy placeholder)

### Input

- `InputManager.h` / `InputManager.cpp`
- `Gamepad.h` / `Gamepad.cpp`

### Combat / projectiles (early prototype)

- `Projectile.h` / `Projectile.cpp` (base class)
- `NormalBulletProjectile.h` / `NormalBulletProjectile.cpp` (simple bullet)

### AI / Movement

- `Steering.h`

- `SeekBehaviour.h` / `SeekBehaviour.cpp`
- `ArriveBehaviour.h` / `ArriveBehaviour.cpp`

This category list is mainly to keep the codebase readable as the number of files grows.
