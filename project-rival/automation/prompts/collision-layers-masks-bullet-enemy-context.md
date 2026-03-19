# Collision system context (layers+masks, bullet hits enemy)

[Back to contents](#contents)

## Contents
1. [Goal of this thread](#goal-of-this-thread)
2. [Current project structure (relevant bits)](#current-project-structure-relevant-bits)
3. [Decisions already made](#decisions-already-made)
4. [Current implementation state (as of this chat)](#current-implementation-state-as-of-this-chat)
5. [What to change next (recommended minimal fixes)](#what-to-change-next-recommended-minimal-fixes)
6. [Notes / pitfalls spotted](#notes--pitfalls-spotted)

---

## Goal of this thread
Establish a **bare-bones** collision system for the SFML game that supports:

- Player fires a bullet
- Bullet hits an enemy
- Enemy **recognizes the collision** (takes damage / reacts)

No environmental collision, triggers, broadphase, or complex response rules yet.

[Back to contents](#contents)

---

## Current project structure (relevant bits)
Ownership in gameplay code:

- `Game` owns enemies: `std::vector<std::unique_ptr<Enemy>> m_enemies;` in `project-rival/Game.h`.
- `Player` owns bullets/projectiles: `std::vector<std::unique_ptr<Projectile>> p_projectiles;` in `project-rival/player.h`.

Implication:

- The simplest place to perform collision checks is in `Game::update(double dt)` **after** player/enemy updates (because `Game` can see both lists).

[Back to contents](#contents)

---

## Decisions already made
Design preferences and decisions agreed during this chat:

- **Filtering:** Use **layers + masks** (bitmask filtering). Avoid a global 2D collision matrix unless strongly justified.
- **Geometry:** Basic AABB overlap is fine for now.
- **Response:** Keep it minimal. Interfaces can be used as **small capability interfaces** (e.g., `IDamageable`) but avoid interface explosion. For now, a simple `takeDamage(int)` is acceptable.
- **Ownership:** Keep bullets owned by `Player` for now; expose them to `Game` for collision checks.

[Back to contents](#contents)

---

## Current implementation state (as of this chat)
### Collision data contract
A shared collision contract exists:

- `project-rival/Collision.h`
  - `enum CollisionLayer` is implemented as **bitflags** (`PLAYER_LAYER`, `ENEMY_LAYER`, `PLAYER_BULLET_LAYER`, `ENEMY_BULLET_LAYER`).
  - `struct CollisionProfile { CollisionLayer layer; uint32_t mask; bool canCollideWith(...) }`
  - `class ICollidable` exposes:
    - `virtual sf::FloatRect getCollisionBounds() const = 0;`
    - `virtual CollisionProfile getCollisionProfile() const = 0;`

### Entities
- `project-rival/player.h` / `project-rival/player.cpp`
  - `Player : public ICollidable`
  - `p_collisionProfile.layer = PLAYER_LAYER;`
  - `p_collisionProfile.mask = ENEMY_LAYER | ENEMY_BULLET_LAYER;`
  - `takeDamage(int)` implemented
  - `getProjectiles()` returns `p_projectiles`

- `project-rival/Enemy.h` / `project-rival/Enemy.cpp`
  - `Enemy : public ICollidable`
  - `e_collisionProfile.layer = ENEMY_LAYER;`
  - `e_collisionProfile.mask = PLAYER_LAYER | PLAYER_BULLET_LAYER;`
  - `takeDamage(int)`, `isDead()`, `onDeath()` implemented

- `project-rival/Projectile.h` / `project-rival/Projectile.cpp`
  - `Projectile : public ICollidable`
  - Constructor takes `bool isFromPlayer` and sets:
    - if from player: `layer = PLAYER_BULLET_LAYER`, `mask = ENEMY_LAYER`
    - else: `layer = ENEMY_BULLET_LAYER`, `mask = PLAYER_LAYER`
  - `applyDamage()` returns `p_damage` (int)
  - `destroy()` sets `p_shouldDestroy = true`

### Where collision is currently performed
- `project-rival/Game.cpp`
  - Collision checks are performed directly inside `Game::update(double dt)`.
  - Geometry test currently uses **center-point containment**, e.g.:
    - `enemyBounds.contains(playerBounds.getCenter())`
    - `bulletBounds.contains(enemyBounds.getCenter())`

This was called out as the main reason the system "technically works" but feels unreliable/unintuitive.

[Back to contents](#contents)

---

## What to change next (recommended minimal fixes)
### 1) Fix geometry test: use bounds overlap, not `contains(center)`
Replace the containment checks with AABB overlap:

- SFML 3: `a.findIntersection(b).has_value()`

### 2) Centralize collision logic into one helper
Add a single reusable function (suggested name):

- `bool areColliding(const ICollidable& a, const ICollidable& b)`

Responsibilities:

- Read both profiles
- Filter using `CollisionProfile::canCollideWith`
- Perform AABB overlap check

Use this helper in `Game::update` for:

- Enemy vs Player
- Bullet vs Enemy

### 3) Bullet vs enemy response logic
On successful collision:

- Enemy: `enemy->takeDamage(bullet->applyDamage())`
- Bullet: `bullet->destroy()` and `break` to prevent multi-hit in the same frame

Bullet cleanup:

- `Player::update` already erases bullets when `shouldDestroy()` becomes true (so bullets are removed next update).

### 4) Fix small correctness issues in `Collision.h`
- `canCollideWith`: compare bitwise results to `0u` (not `CollisionLayer::NONE` on an unscoped enum).
- Use `1u << n` for enum constants.
- Consider default initialization for `CollisionProfile` fields.

### 5) Fix `Player::getProjectiles()` declaration
In `project-rival/player.h`, the return type was observed as `std::vector<std::unique_ptr<Projectile>>& const getProjectiles();` which should be corrected to:

- `std::vector<std::unique_ptr<Projectile>>& getProjectiles();`

[Back to contents](#contents)

---

## Notes / pitfalls spotted
- Center containment misses many overlaps and produces false negatives/positives.
- After calling `bullet->destroy()`, ensure the bullet doesn’t keep checking and hitting multiple enemies in the same frame (add `if (bullet->shouldDestroy())` guard and/or `break`).
- Dead enemy cleanup: currently enemies are erased during the main enemy update loop. If bullets can kill enemies after that loop, those deaths may not be erased until the next frame unless an additional cleanup pass is added.
- `Collision.h` include path uses `"Sfml/Graphics.hpp"` (case mismatch vs `SFML`)—works on Windows, but not portable.

[Back to contents](#contents)
