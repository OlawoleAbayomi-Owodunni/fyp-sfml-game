# 2) Player

[Back to architecture index](README.md)

`player` (in `player.h/.cpp`) represents the controllable character.

## Responsibilities (current)

- Owns its SFML shape (`p_body`)
- Reads input (keyboard or gamepad, via `InputManager`)
- Moves by updating velocity and applying it every update
- Exposes its position (used as an enemy target)

Also (current prototype features):

- Aiming (mouse or right stick)
- Shooting simple projectiles

## Movement (high level)

Movement is handled in `player::handleMovement(double dt)`:

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
