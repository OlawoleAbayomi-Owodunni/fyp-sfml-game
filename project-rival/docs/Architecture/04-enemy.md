# 4) Enemies (current state)

[Back to architecture index](README.md)

The project currently has an enemy **base class** (`Enemy`) and a couple of concrete enemy types.

## `Enemy` (base class)

`Enemy` (in `Enemy.h/.cpp`) is an **abstract base class** (it has pure virtual functions), so it cannot be instantiated directly.

It provides:

- a visible SFML body (`sf::RectangleShape`)
- common setup via `initBody(...)`
- a shared `render(...)`
- a shared `e_target` member that derived enemies can use

It also implements `ICollidable` and includes basic “combat” state:

- collision profile (layer/mask)
- health (`e_health` / `e_maxHealth`, via `takeDamage(...)`)
- death flag (`isDead()`)

It also provides `reset()` (restore start position and health).

Currently, enemies collide with:

- the player
- player-fired bullets
- walls (`WALL_LAYER`)

Derived classes must implement:

- `init()`
- `update(double dt)`
- `setTarget(const sf::Vector2f& target)`
- `hitWall(oldPos)` (wall collision response)

## `GruntEnemy`

`GruntEnemy` (in `GruntEnemy.h/.cpp`) is a steering-driven moving enemy.

- Owns a `SteeringAgent`
- Owns both `SeekBehaviour` and `ArriveBehaviour`
- Auto-switches between behaviours based on distance to target (hysteresis via enter/exit distances)

## `TurretEnemy`

`TurretEnemy` (in `TurretEnemy.h/.cpp`) is currently a placeholder enemy that:

- has a body
- stores a target (`setTarget` updates `e_target`)
- does not yet implement any attack or aiming logic

## How enemies are used in the game

In `Game`, enemies are stored as `std::vector<std::unique_ptr<Enemy>>`.

Each update:

- the target is set to the player position (`enemy->setTarget(m_player.getPosition())`)
- then each enemy is updated (`enemy->update(dt)`)

Enemies are spawned when the current room is generated (based on room spawner tiles).

Collision + damage (current approach):

- If player collides with an enemy, the player takes damage.
- Player projectiles are checked against enemies; on hit:
  - enemy takes damage
  - projectile is destroyed
- Enemies are checked against wall colliders; on hit, `enemy->hitWall(oldPos)` is called (currently rollback).
- Dead enemies are removed from the `m_enemies` list.

## Likely next steps

- Add actual turret behaviour (aiming / firing / cooldown) for `TurretEnemy`.
- Consider making enemy tuning values configurable (speed/acceleration/radii) per enemy type.
