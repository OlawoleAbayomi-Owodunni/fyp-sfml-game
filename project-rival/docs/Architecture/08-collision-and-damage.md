# 8) Collision + damage (current approach)

[Back to architecture index](README.md)

This project uses a simple collision system with **layers** and **masks** to filter which objects can collide.

## Collision layers and masks

Defined in `Collision.h`:

- `CollisionLayer` (bit flags)
  - examples: `PLAYER_LAYER`, `ENEMY_LAYER`, `PLAYER_BULLET_LAYER`
- `CollisionProfile`
  - `layer`: what this object *is*
  - `mask`: what this object *can collide with*

Two objects are allowed to collide if:

- A’s mask contains B’s layer **and**
- B’s mask contains A’s layer

## `ICollidable` (capability interface)

Also in `Collision.h`:

- `ICollidable` requires:
  - `getCollisionBounds()` (an `sf::FloatRect`)
  - `getCollisionProfile()`

Current gameplay types that implement it:

- `Player`
- `Enemy` (and all enemy subclasses)
- `Projectile` (and bullet subclasses)

## Collision checks

`CollisionCheck::areColliding(a, b)` (in `Collision.cpp`) does:

1. layer/mask filtering (`CollisionProfile::canCollideWith`)
2. bounding box intersection test (SFML rectangles)

## Damage flow (high level)

Right now, the main damage logic lives in `Game::update(dt)`:

- Player vs enemy collision → `m_player.takeDamage(...)`
- Player bullets vs enemies:
  - `enemy->takeDamage(bullet->applyDamage())`
  - `bullet->destroy()`
- If an enemy reports `isDead()` it is removed from the enemies list.
