# Development log (weekly / milestone)

This document tracks technical progress over time in a supervisor-friendly way.

## How to use this log ---->DELETE WHEN PROJECT IS COMPLETE<-----

- Add a new section each **week** (recommended) or per **milestone**.
- Each entry should be short:
  - **Summary**: 2–5 bullets
  - **Technical notes** (optional): 2–6 bullets
  - **Evidence**: commit hashes
  - **Why / intent**: add when available

---

## Week of 2026-01-22

### Summary
- Added Windows controller (XInput) support and integrated it into player controls.
- Improved player movement so it supports omni-directional input (keyboard and controller stick).

### Technical notes
- Introduced `Gamepad` abstraction to read sticks/triggers/buttons and apply deadzones.
- Added `InputManager` as a simple static access point for reading controller state.
- Updated `player` to read controller input and (optionally) trigger rumble.

### Evidence (commits)
- `a1be98e` — Controller Support
- `f2663af` — Player: Omni Directional movement

---

## Week of 2026-01-21

### Summary
- Wired the `player` entity into the game so it updates and renders during gameplay.
- Implemented/iterated on basic player movement.
- Synced work with `main` via merge.

### Technical notes
- `Game` now owns a `player` instance and calls its `update(dt)` and `render(...)`.
- Movement logic was iterated in `player.cpp`.

### Evidence (commits)
- `cb409c7` — Player: called in game
- `c324a0b` — Player class: movement
- `1f10f8e` — Player class: movement
- `1aecc43` — merge branch 'main'

---

## Week of 2025-11-24 (project setup)

### Summary
- Initial SFML project setup, solution/project files, and base game scaffolding.
- Added basic assets (font/logo) and a `.gitignore`.

### Evidence (commit)
- `ca2c920` — Setup + `.gitignore`

---
