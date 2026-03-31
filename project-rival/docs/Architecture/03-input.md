# 3) Input (high level)

[Back to architecture index](README.md)

The input code is split into two parts:

- `InputManager`: a static helper that owns a `Gamepad`
- `Gamepad`: a small wrapper around XInput

## `InputManager`

Typical usage:

- call `InputManager::update()` once per frame
- then read input through `InputManager::pad()`

In `Game::update(dt)`, the current mouse position is also read from the window and passed into `Player::update(...)` for aiming.

## `Gamepad`

Exposes:

- sticks as `sf::Vector2f` in `[-1..1]`
- triggers in `[0..1]`
- button states (`down`, `pressed`, `released`)
- rumble (`setRumble`)

Current gameplay usage examples:

- left stick: movement
- right stick or mouse: aiming
- right trigger or left mouse button: shoot

High-level game controls (in `Game::gameInput()`):

- Exit: `Escape` or controller `Start`
- Restart run: `R` or controller `Select`
- Toggle camera mode (player cam vs floor overview): `V` or controller `DPadDown`

Also applies deadzones to reduce controller drift.
