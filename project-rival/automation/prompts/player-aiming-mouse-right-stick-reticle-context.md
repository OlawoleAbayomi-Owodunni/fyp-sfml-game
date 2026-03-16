# Context: Player aiming (twin-stick + mouse) with unified aim direction + reticle

## Contents
- [Goal](#goal)
- [Repo / project notes](#repo--project-notes)
- [Relevant files](#relevant-files)
- [Current behavior (before aiming changes)](#current-behavior-before-aiming-changes)
- [Agreed approach: unify to an aim direction](#agreed-approach-unify-to-an-aim-direction)
- [Proposed implementation (incremental step 1)](#proposed-implementation-incremental-step-1)
- [Follow-up steps](#follow-up-steps)

## Goal
Implement a **twin-stick shooter aiming system** where:
- Left stick / WASD moves.
- Right stick aims.
- Mouse aims.
- A **reticle** shows aim direction (orbiting around the player).
- Aim output is **consistent** between mouse and controller so later bullet firing can use the same data.

## Repo / project notes
- Project is an SFML game (SFML 3-style event API is in use: `event->is<sf::Event::Closed>()`).
- `dt` is passed as **seconds** from `Game::update` (see `.github/copilot-instructions.md`).
- Code style uses lower-case `player` class name and `p_` prefixes for member variables.
- Input/gamepad uses XInput via a wrapper class.

## Relevant files
- `project-rival/project-rival/Game.cpp` / `project-rival/project-rival/Game.h`
- `project-rival/project-rival/player.cpp` / `project-rival/project-rival/player.h`
- `project-rival/InputManager.cpp` / `project-rival/InputManager.h`
- `project-rival/project-rival/Gamepad.cpp` / `project-rival/project-rival/Gamepad.h`

## Current behavior (before aiming changes)
- `Game::update(double dt)` calls:
  - `InputManager::update();`
  - `m_player.update(dt);`
- `player::handleMovement(double dt)`:
  - Uses `InputManager::pad().leftStick()` when non-zero.
  - Otherwise uses keyboard (WASD).
- No aiming system exists yet; trigger rumble is tested in `player::update`.

## Agreed approach: unify to an aim direction
To keep both input schemes consistent, standardize aiming output to a single vector:

- **Aim direction** (`sf::Vector2f p_aimDir`): a unit vector in world-space.

Then derive:
- **Reticle position**: `playerPos + p_aimDir * reticleDistance`
- **Bullet velocity** later: `p_aimDir * bulletSpeed`

Why:
- Right stick naturally outputs a direction (no absolute position).
- Mouse naturally outputs a world position, but direction is computed via:
  - `mouseWorld - playerPos`

### Active aim device switching (important)
Aiming should not snap to the mouse whenever the right stick returns to center.
So track which device is currently “active”:
- If right stick magnitude is non-zero → controller aim becomes active.
- If mouse moves → mouse aim becomes active.
- If controller aim is active and stick is centered → keep the last `p_aimDir`.

## Proposed implementation (incremental step 1)
> Note: in the original chat, this was proposed as the first small step; apply as code changes in a follow-up instance.

1. Add to `player`:
   - `sf::Vector2f p_aimDir{ 1.f, 0.f };`
   - `sf::CircleShape p_reticle;`
   - `float p_reticleDistance{ 90.f };`
   - `bool p_isControllerAiming{ false };`
   - `sf::Vector2f p_prevMouseWorld{ 0.f, 0.f };`
   - A new private `handleAiming(const sf::Vector2f& mouseWorld)`.

2. Pass `mouseWorld` from `Game::update`:
   - `sf::Vector2f mouseWorld = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));`
   - Change call to `m_player.update(dt, mouseWorld);`

3. Implement reticle orbit behavior:
   - `p_reticle.setPosition(p_body.getPosition() + p_aimDir * p_reticleDistance);`

4. (Optional but helpful) expose:
   - `player::getAimDirection()` for bullets later.

## Follow-up steps
- Decide reticle behavior for mouse:
  1) keep orbiting at fixed radius (same as controller), or
  2) place reticle exactly at the mouse cursor position.

- Add a bullet class that uses the unified aim output:
  - `spawnPos = playerPos + p_aimDir * muzzleOffset`
  - `bulletVel = p_aimDir * bulletSpeed`

- If/when a camera/view is added, keep mouse mapping in `Game` using `mapPixelToCoords` so aiming remains correct.
