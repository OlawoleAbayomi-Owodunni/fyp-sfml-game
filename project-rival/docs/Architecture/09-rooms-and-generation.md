# 9) Rooms + procedural generation (current state)

[Back to architecture index](README.md)

The project has an early “rooms” system built around a **data plan → runtime instance** workflow.

## `RoomPlan` (data)

Defined in `RoomBlueprint.h`:

- `RoomPlan` describes a room **without any SFML objects**.
- It contains:
  - metadata: `id`, `type`, `seed`
  - size: `width`, `height`, `tileSize`
  - layout: `tileMap` (1D array), plus `doors`, `spawners`, and `triggers`
  - room state: `isCleared` (used by combat rooms)

The tile map uses a 1D index:

- `index = row * width + column`

Helper functions:

- `getTile(row, col)`
- `setTile(row, col, type)`

## Room generators (`IRoomGenerator`)

`IRoomGenerator` is an interface that can create a `RoomPlan`.

Current implementations:

- `CombatRoom` (`CombatRoom.h/.cpp`)
  - generates outer walls
  - randomly chooses a room size (currently: height ~10–13, width ~15–20)
  - randomly places interior obstacles (“pillars”)
  - generates doors (currently hard-coded directions)
  - supports combat waves via `generateNewWave(...)` and `setRoomCleared(...)`
- `SpawnRoom` (`SpawnRoom.h/.cpp`)
  - fixed-size room (currently 11x11)
  - generates a player spawn marker
  - generates doors
- `PortalRoom` (`PortalRoom.h/.cpp`)
  - fixed-size room (currently 11x11)
  - generates a portal spawn marker + portal trigger
  - generates doors

Obstacle/spawn counts in combat rooms are derived from the interior size (via `interiorArea`).

Note: `RoomPlan` includes a `seed` field, but room generation currently uses `rand()` for most details.

Doors:

- Doors are generated as 2–3 tiles wide (based on room width/height parity).
- Door tiles also add `DoorTrigger` entries so `Game` can react to entering a doorway.

## `RoomInstance` (runtime)

`RoomInstance` (`RoomInstance.h/.cpp`) builds SFML objects from a `RoomPlan`:

- wall shapes (for rendering)
- wall colliders (`StaticCollision`) with `WALL_LAYER`
- small debug shapes for spawner tiles

Anything marked as `Tile::WALL` becomes a wall shape + a wall collider (outer walls and interior obstacles).

Doors and triggers:

- `Tile::DOOR` is rendered as a door shape.
- If a door is marked locked in `RoomPlan.doors`, `RoomInstance` adds a `DOOR_LAYER` collider so it blocks movement/projectiles.
- Triggers are rendered as debug shapes and added as colliders:
  - portal trigger → `PORTAL_TRIGGER_LAYER`
  - door trigger → `DOOR_TRIGGER_LAYER`

This is where room geometry becomes “real” (drawn/collidable) in the world.

## Game integration (current)

In `Game`:

- `generateRoom(roomPlan)` builds a `RoomInstance` from a `RoomPlan`.
- Spawner tiles in the plan are used to spawn runtime objects:
  - spawn room spawner → sets the player spawn position
  - combat room spawners → spawn enemies
  - portal room spawner → (visual marker only for now)
- The room instance is rendered before the entities.

Combat room flow (current prototype):

- Entering a combat room door trigger starts combat (waves).
- Combat waves spawn enemies and lock doors.
- When waves are cleared, doors unlock and the room is marked cleared.

## Likely next steps

- Decide what parts of generation should be deterministic (seed-based) vs “random each run”.
- Add door geometry and room-to-room connections.
- Add collision resolution against wall colliders (right now walls are generated + colliders exist, but movement resolution may still be pending).
