# 2) Player

[Back to architecture index](README.md)

`Player` (declared in `player.h` / implemented in `player.cpp`) represents the controllable character.

## Responsibilities (current)

- Owns its SFML body + reticle (`p_body`, `p_reticle`).
- Reads movement/aiming/fire input (keyboard + mouse and gamepad via `InputManager`).
- Handles movement, aim-direction selection, and weapon switching.
- Implements `ICollidable` for collision checks.
- Tracks health/death state (`p_health`, `p_maxHealth`, `p_isDead`).
- Applies basic collision response via `hitWall()` (rollback to previous position).

## Movement (high level)

Movement is handled in `Player::handleMovement(float dt)`:

- build a movement direction from either:
  - gamepad left stick, or
  - keyboard WASD
- normalize where appropriate
- apply `p_velocity = direction * p_moveSpeed`
- move via `p_body.move(p_velocity * dt)`

`dt` is expected to be in seconds.

## Aiming + weapons (high level)

Aiming is handled in `Player::handleAiming(const Vector2f mousePos)`:

- if right stick is active, controller aiming is used
- if mouse movement is detected, mouse aiming is used
- aim direction is normalized and used to place the reticle

Weapon flow:

- Player owns a weapon list (`Pistol`, `AR`, `Shotgun`, `Knife`, `Sword`, `Axe`).
- `RightBumper` / `LeftBumper` cycles current weapon.
- Fire input (`rightTrigger` or left mouse) dispatches through current weapon:
  - ranged weapons spawn `Projectile` instances into the game projectile list
  - melee weapons spawn short-lived `DamageTrigger` instances

## Collision profile (current)

The player collision profile includes collisions with:

- `ENEMY_LAYER`
- `ENEMY_BULLET_LAYER`
- `WALL_LAYER`
- `DOOR_LAYER`
- `PORTAL_TRIGGER_LAYER`
- `DOOR_TRIGGER_LAYER`
- `DAMAGE_TRIGGER_LAYER`

## Used by other systems

- `Game` passes runtime containers into `Player::update(...)` so player weapons can emit projectiles/triggers.
- Enemies use player position as their target (`m_player.getPosition()`).
- `Game` checks `m_player.isDead()` and restarts the run when the player dies.
