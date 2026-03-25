# 9) Rooms + procedural generation (current state)

[Back to architecture index](README.md)

The project has an early “rooms” system built around a **data plan → runtime instance** workflow.

## `RoomPlan` (data)

Defined in `RoomBlueprint.h`:

- `RoomPlan` describes a room **without any SFML objects**.
- It contains:
  - metadata: `id`, `type`, `seed`
  - size: `width`, `height`, `tileSize`
  - layout: `tileMap` (1D array), plus `doors` and `spawners`

The tile map uses a 1D index:

- `index = row * width + column`

Helper functions:

- `getTile(row, col)`
- `setTile(row, col, type)`

## Room generators (`IRoomGenerator`)

`IRoomGenerator` is an interface that can create a `RoomPlan`.

Current implementation:

- `CombatRoom` (`CombatRoom.h/.cpp`)
  - generates outer walls
  - randomly places interior obstacles (“pillars”)
  - randomly places enemy spawners

Note: `RoomPlan` includes a `seed` field, but `CombatRoom` currently uses `rand()` for generation.

## `RoomInstance` (runtime)

`RoomInstance` (`RoomInstance.h/.cpp`) builds SFML objects from a `RoomPlan`:

- wall shapes (for rendering)
- wall colliders (`StaticCollision`) with `WALL_LAYER`
- small debug shapes for spawner tiles

This is where room geometry becomes “real” (drawn/collidable) in the world.

## Likely next steps

- Decide what parts of generation should be deterministic (seed-based) vs “random each run”.
- Add door geometry and room-to-room connections.
- Add collision resolution against wall colliders (right now walls are generated + colliders exist, but movement resolution may still be pending).
