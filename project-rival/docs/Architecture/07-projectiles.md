# 7) Projectiles (prototype)

[Back to architecture index](README.md)

The project has a projectile subsystem used by both player and enemy weapons.

## `Projectile` (base class)

`Projectile` (`Projectile.h/.cpp`) is an abstract base class.

It provides:

- a visible body (`sf::RectangleShape`)
- lifetime tracking (`p_lifetime` + `onExpire(dt)`)
- collision support via `ICollidable`
- damage output via `applyDamage()`
- destruction signaling via `shouldDestroy()` / `destroy()`

Collision profile setup depends on source:

- player-fired: `PLAYER_BULLET_LAYER`, mask includes `ENEMY_LAYER`
- enemy-fired: `ENEMY_BULLET_LAYER`, mask includes `PLAYER_LAYER`
- both also collide with `WALL_LAYER` and `DOOR_LAYER`

## `NormalBulletProjectile`

`NormalBulletProjectile` is the current concrete bullet:

- normalizes direction in `init()`
- moves by `direction * speed * dt`
- expires by lifetime or collision destruction

## Current runtime ownership

Projectiles are owned at `Game` level in `m_gameProjectiles`.

- Player weapons append projectiles into this shared list.
- Turret enemies also append projectiles into this shared list.
- `Game::update(dt)` updates/removes projectiles and applies collision-driven damage.

## Damage values (current)

Damage is defined by the weapon creating the projectile (not by projectile type alone). For example:

- player pistol / AR / shotgun pass different values
- enemy pistol also uses its own configured value
