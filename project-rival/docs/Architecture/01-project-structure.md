# 1) Project structure

[Back to architecture index](README.md)

Most gameplay code is inside `project-rival/`.

## Suggested mental model (categories)

### Entities

- `player.h` / `player.cpp`
- `Enemy.h` / `Enemy.cpp`

### Input

- `InputManager.h` / `InputManager.cpp`
- `Gamepad.h` / `Gamepad.cpp`

### AI / Movement

- `Steering.h`

- `SeekBehaviour.h` / `SeekBehaviour.cpp`
- `ArriveBehaviour.h` / `ArriveBehaviour.cpp`

This category list is mainly to keep the codebase readable as the number of files grows.
