# 3) Input (high level)

[Back to architecture index](README.md)

The input code is split into two parts:

- `InputManager`: a static helper that owns a `Gamepad`
- `Gamepad`: a small wrapper around XInput

## `InputManager`

Typical usage:

- call `InputManager::update()` once per frame
- then read input through `InputManager::pad()`

## `Gamepad`

Exposes:

- sticks as `sf::Vector2f` in `[-1..1]`
- triggers in `[0..1]`
- button states (`down`, `pressed`, `released`)
- rumble (`setRumble`)

Also applies deadzones to reduce controller drift.
