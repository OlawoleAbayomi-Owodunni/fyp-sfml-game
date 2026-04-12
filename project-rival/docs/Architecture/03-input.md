# 3) Input (high level)

[Back to architecture index](README.md)

The input code is split into two parts:

- `InputManager`: a static helper that owns a `Gamepad`
- `Gamepad`: a small wrapper around XInput

## `InputManager`

Typical usage:

- call `InputManager::update()` once per frame
- then read input through `InputManager::pad()`

In `Game::update(dt)`, mouse input is converted using the currently active gameplay view (`player` camera or `floor` camera), then passed into `Player::update(...)` as both world and pixel coordinates.

## `Gamepad`

Exposes:

- sticks as `sf::Vector2f` in `[-1..1]`
- triggers in `[0..1]`
- button states (`down`, `pressed`, `released`)
- rumble (`setRumble`)

Current gameplay usage examples:

- left stick: movement
- right stick or mouse: aiming
- right trigger or left mouse button: attack
- left/right bumper: cycle equipped weapon

High-level game controls:

- Keyboard event controls (`Game::processGameEvents(...)`):
  - Exit: `Escape`
  - Restart / start run: `R`
  - Toggle camera mode (player cam vs floor overview): `V`
  - Debug LLM room prompt: `Num0`
- Controller controls (`Game::ControllerInputHandler()`):
  - Exit: `Start`
  - Restart / start run: `Select`
  - Toggle camera mode: `DPadDown`
  - Debug LLM room prompt: `DPadUp`

Contextual interactions:

- Use portal (when standing inside a portal trigger): `Space` or controller `A`

Also applies deadzones to reduce controller drift.
