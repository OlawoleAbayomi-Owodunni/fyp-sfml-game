# 4) Enemy (current state)

[Back to architecture index](README.md)

`Enemy` (in `Enemy.h/.cpp`) is a simple entity that demonstrates steering-based movement.

## What exists

- A visible SFML body (`sf::RectangleShape`)
- A `SteeringAgent` (`position`, `velocity`, `maxSpeed`, `maxAcceleration`)
- A current steering behaviour (`ISteeringBehaviour*`), currently set to `ArriveBehaviour` by default
- `init()`, `update(double dt)`, `render(...)`
- `setTarget(...)` to update the target used by behaviours
- `setBehaviour(...)` to switch behaviours at runtime (for testing)

In `Game::update(dt)`, the enemy target is set to the player position each frame.

## Likely next steps

- Decide when to use `SeekBehaviour` vs `ArriveBehaviour` (for example: seek when far away, arrive when close)
- Expose behaviour parameters (speed/acceleration/radii) so different enemy “types” can be created
