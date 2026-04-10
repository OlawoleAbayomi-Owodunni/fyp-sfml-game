# 4) Enemies (current state)

[Back to architecture index](README.md)

The project currently has an enemy base class (`Enemy`) and two concrete enemy types.

## `Enemy` (base class)

`Enemy` (`Enemy.h/.cpp`) is an abstract base class.

It provides:

- shared SFML body setup/rendering (`initBody(...)`, `render(...)`)
- shared target state (`e_target`)
- shared collision profile (`ENEMY_LAYER`)
- shared health/death state (`e_health`, `e_maxHealth`, `e_isDead`)

It also implements `ICollidable` and exposes common behavior like `takeDamage(...)`, `isDead()`, and `reset()`.

Derived classes implement:

- `init()`
- `update(float dt)`
- `setTarget(...)`
- `hitWall()`

## `GruntEnemy`

`GruntEnemy` is a steering-driven mover:

- owns a `SteeringAgent`
- switches between `SeekBehaviour` and `ArriveBehaviour` using distance hysteresis
- uses an `EnemyWeapon` (`KNIFE`) while in arrive range to spawn melee `DamageTrigger`s

## `TurretEnemy`

`TurretEnemy` is a stationary attacker:

- tracks the player target direction
- updates and renders its `EnemyWeapon` (`PISTOL`)
- fires projectiles on a randomized cooldown timer

## Game integration

`Game` stores enemies in `std::vector<std::unique_ptr<Enemy>>`.

Per update:

- each enemy target is set to player position
- concrete enemy update path is selected (`TurretEnemy` projectile update, `GruntEnemy` melee-trigger update)
- dead enemies are removed from the vector

Collisions/damage are resolved in `Game::CollisionChecks()`:

- bullets/triggers can damage enemies
- enemies collide with walls and locked doors (`hitWall()` handling)
