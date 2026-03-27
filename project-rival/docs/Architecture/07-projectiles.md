# 7) Projectiles (prototype)

[Back to architecture index](README.md)

The project has an early projectile system used for simple shooting.

## `Projectile` (base class)

`Projectile` (in `Projectile.h/.cpp`) is an abstract base class.

It provides:

- a visible body (`sf::RectangleShape`)
- a lifetime timer (`p_lifetime`)
- `shouldDestroy()` so owners can remove expired projectiles
- `render(...)`

It also implements `ICollidable` so projectiles can participate in collision checks.

Projectiles also set a collision profile based on who fired them:

- player-fired bullets collide with enemies
- enemy-fired bullets collide with the player

They also collide with walls (`WALL_LAYER`) & with locked doors (`DOOR_LAYER`) and are destroyed on impact.

Derived projectiles are expected to:

- implement `init()`
- implement `update(double dt)`
- call `onExpire(dt)` if they want lifetime-based cleanup

## `NormalBulletProjectile`

`NormalBulletProjectile` is a simple bullet:

- stores a direction
- moves at a fixed speed each update
- expires after a short lifetime

It also sets a basic damage value (`applyDamage()`).

(Current `NormalBulletProjectile` damage is set to 10.)

## Current ownership

Right now, the `Player` owns a `std::vector<std::unique_ptr<Projectile>>` and spawns bullets when the player shoots.

This is marked in code as “test” and can later be moved to a weapon system.
