# 2) Player

[Back to architecture index](README.md)

`Player` (declared in `player.h` / implemented in `player.cpp`) represents the controllable character.

## Responsibilities (current)

- Owns its SFML shape (`p_body`)
- Reads input (keyboard or gamepad, via `InputManager`)
- Moves by updating velocity and applying it every update
- Exposes its position (used as an enemy target)

Also:

- Implements `ICollidable` (so it can participate in collision checks)
- Has health (`p_health` / `p_maxHealth`) + `takeDamage(...)` (currently resets when health reaches 0)
- Has basic wall collision response via `hitWall(oldPos)` (currently a rollback)

Currently, the player collides with:

- enemies
- walls (`WALL_LAYER`)
- triggers (`PORTAL_TRIGGER_LAYER`)

Also (current prototype features):

- Aiming (mouse or right stick)
- Shooting simple projectiles

## Movement (high level)

Movement is handled in `Player::handleMovement(double dt)`:

- build a `direction` vector from input
  - controller: use `InputManager::pad().leftStick()`
  - keyboard: WASD
- normalize it so diagonal movement isn’t faster
- set `p_velocity = direction * p_moveSpeed`
- move by `p_velocity * dt`

Note: `dt` is expected to be **seconds**.

## Aiming + shooting (high level)

- Aiming:
  - controller: uses the right stick (`InputManager::pad().rightStick()`)
  - mouse: uses the direction from player → mouse position
- A small “reticle” is drawn in front of the player to show aim direction.
- Shooting:
  - right trigger or left mouse button spawns a `NormalBulletProjectile`
  - projectiles are currently owned by the player (temporary; likely to move into a weapon system later)

## Used by other systems

Enemies use the player position as a target (via `m_player.getPosition()`).

Game code also reads the player's projectiles via `m_player.getProjectiles()` for bullet-vs-enemy collision.
