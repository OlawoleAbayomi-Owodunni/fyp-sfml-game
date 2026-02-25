# 2) Player

[Back to architecture index](README.md)

`player` (in `player.h/.cpp`) represents the controllable character.

## Responsibilities (current)

- Owns its SFML shape (`p_body`)
- Reads input (keyboard or gamepad)
- Moves by updating velocity and applying it every update

## Movement (high level)

Movement is handled in `player::handleMovement(double dt)`:

- build a `direction` vector from input
- normalize it so diagonal movement isn’t faster
- set `p_velocity = direction * p_moveSpeed`
- move by `p_velocity * dt`

Note: `dt` is expected to be **seconds**.
