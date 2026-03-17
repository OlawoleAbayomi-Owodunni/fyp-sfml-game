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

Derived projectiles are expected to:

- implement `init()`
- implement `update(double dt)`
- call `onExpire(dt)` if they want lifetime-based cleanup

## `NormalBulletProjectile`

`NormalBulletProjectile` is a simple bullet:

- stores a direction
- moves at a fixed speed each update
- expires after a short lifetime

## Current ownership

Right now, the `player` owns a `std::vector<std::unique_ptr<Projectile>>` and spawns bullets when the player shoots.

This is marked in code as “test” and can later be moved to a weapon system.
