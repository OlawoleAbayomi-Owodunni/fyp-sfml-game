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
  - randomly chooses a room size (currently: height ~10–13, width ~15–20)
  - randomly places interior obstacles (“pillars”)
  - randomly places enemy spawners

Obstacle/spawn counts are derived from the interior size (via `interiorArea`).

Note: `RoomPlan` includes a `seed` field, but `CombatRoom` currently uses `rand()` for generation.

## `RoomInstance` (runtime)

`RoomInstance` (`RoomInstance.h/.cpp`) builds SFML objects from a `RoomPlan`:

- wall shapes (for rendering)
- wall colliders (`StaticCollision`) with `WALL_LAYER`
- small debug shapes for spawner tiles

Anything marked as `Tile::WALL` becomes a wall shape + a wall collider (outer walls and interior obstacles).

This is where room geometry becomes “real” (drawn/collidable) in the world.

## Game integration (current)

In `Game`:

- `generateRoom()` creates a `RoomPlan` and builds a `RoomInstance`.
- Enemy spawner tiles in the plan are used to spawn actual `Enemy` instances.
- The room instance is rendered before the entities.

## Likely next steps

- Decide what parts of generation should be deterministic (seed-based) vs “random each run”.
- Add door geometry and room-to-room connections.
- Add collision resolution against wall colliders (right now walls are generated + colliders exist, but movement resolution may still be pending).
