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

- `RoomBlueprint.h` (RoomPlan + tiles + spawners + generator interface)
- `CombatRoom.h` / `CombatRoom.cpp` (combat room generator)
- `RoomInstance.h` / `RoomInstance.cpp` (builds a drawable room + wall colliders)
- `StaticCollision.h` / `StaticCollision.cpp` (colliders for walls/geometry)

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
