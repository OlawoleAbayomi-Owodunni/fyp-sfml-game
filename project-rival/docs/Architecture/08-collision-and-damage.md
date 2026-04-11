# 8) Collision + damage (current approach)

[Back to architecture index](README.md)

This project uses a simple collision system with **layers** and **masks** to filter which objects can collide.

## Collision layers and masks

Defined in `Collision.h`:

- `CollisionLayer` bitflags (player, enemy, bullets, walls, doors, triggers)
- `CollisionProfile`
  - `layer`: what this object *is*
  - `mask`: what this object *can collide with*

Two objects are allowed to collide if:
- A's mask contains B's layer **and**
- B's mask contains A's layer

## `ICollidable`

`ICollidable` requires:

- `getCollisionBounds()`
- `getCollisionProfile()`

Current gameplay implementations include:

- `Player`
- `Enemy` hierarchy
- `Projectile` hierarchy
- `DamageTrigger`
- `StaticCollision`

## Collision checks

`CollisionCheck::areColliding(a, b)` performs:

1. layer/mask filtering (`CollisionProfile::canCollideWith`)
2. AABB intersection (`sf::FloatRect` intersection)

## Environment colliders

`RoomInstance` builds static colliders for:

- walls (`WALL_LAYER`)
- locked doors (`DOOR_LAYER`)
- portal triggers (`PORTAL_TRIGGER_LAYER`)
- door triggers (`DOOR_TRIGGER_LAYER`)

Portal triggers are interaction volumes:

- in `HUB` mode: interaction starts a dungeon run
- in `DUNGEON` mode: interaction requests the next floor

## Collision response (current)

Handled in `Game::CollisionChecks()`:

- player/enemy wall-door collisions use rollback (`hitWall()`)
- projectiles are destroyed on wall/door impact
- door triggers can start combat waves (in uncleared combat rooms)
- portal triggers can start runs / request floor advancement (by mode)

## Damage flow (high level)

Also handled in `Game::CollisionChecks()`:

- projectile -> enemy: enemy takes `applyDamage()`, projectile destroyed
- projectile -> player: player takes `applyDamage()`, projectile destroyed
- damage trigger -> player/enemy: target takes trigger `damage()`

`Game::update(dt)` then removes destroyed projectiles/triggers and removes dead enemies.
