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

`RoomType` currently also includes `CORRIDOR`, which is used for corridor “rooms” generated at runtime to connect doorways between rooms.

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
  - supports combat waves via `generateNewWave(...)` and `setRoomCleared(...)`
- `SpawnRoom` (`SpawnRoom.h/.cpp`)
  - fixed-size room (currently 11x11)
  - generates a player spawn marker
- `PortalRoom` (`PortalRoom.h/.cpp`)
  - fixed-size room (currently 11x11)
  - generates a portal spawn marker + portal trigger

Obstacle/spawn counts in combat rooms are derived from the interior size (via `interiorArea`).

Note: `RoomPlan` includes a `seed` field, but room generation currently uses `rand()` for most details.

Door tiles are not placed by the room generators themselves. Doors are applied later based on floor connectivity (see below).

## Floors (room graph + layout) (prototype)

Rooms are now also used as part of a simple **floor pipeline**:

- `FloorPlan` (`FloorBlueprint.h`):
  - nodes = rooms (`id`, `RoomType`)
  - edges = connections between rooms
  - adjacency list is built from edges for neighbour queries
- `FloorGenerator` (`FloorGenerator.h/.cpp`): creates a deterministic room graph from a floor seed (derived from a dungeon seed + floor id).
- `FloorLayoutGenerator` (`FloorLayout.h/.cpp`): embeds the graph into a simple 2D grid (tile-grid positions per room).

Current generation constraints:

- FloorGenerator currently chooses a random room count in `[3..10]` using `std::mt19937`.
- Room 0 is always `SPAWN`.
- The last room is always `PORTAL` (boss room is planned for final floors).
- All middle rooms currently resolve to `COMBAT` (the distribution range currently only spans `COMBAT`).
- The generator avoids directly connecting spawn and portal rooms with a single edge.

Door placement from floor connectivity:

- Each room plan is generated first.
- Doors are then cleared and re-added based on `FloorPlan.edges`.
- `RoomDoorUtils` is responsible for adding doors and ensuring:
  - no duplicate door per direction
  - door tiles span 2–3 tiles (based on room parity)
  - `DoorTrigger` markers are added for door tiles

Corridors:

- `Game::buildCorridors()` builds corridor `RoomPlan`s from floor edges.
- Each corridor is a small wall “tube” with a walkable strip of `Tile::FLOOR` down the middle.
- Corridor plans are immediately converted to `RoomInstance` objects and appended to `m_roomInstances`.

## Dungeon progression (prototype)

`DungeonPlan` (`DungeonPlan.h`) tracks:

- `seed`
- `currentFloorId`
- `floorCount`
- `isDungeonComplete`

In `Game`:

- `gameStart()` initializes a new dungeon plan (currently using a fixed seed for testing) and calls `loadNewFloor()`.
- Standing in a portal trigger and pressing interact (`Space` / `A`) sets a `m_requestNextFloor` flag.
- `Game::update(dt)` consumes the flag and advances the dungeon:
  - if the dungeon is complete → close window (temporary)
  - otherwise → load the next floor via `loadNewFloor()`

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

- `generateRoom(roomId)` builds a `RoomInstance` from the corresponding `RoomPlan` and its computed world position.
- Spawner tiles in the plan are used to spawn runtime objects:
  - spawn room spawner → sets the player spawn position (`spawnPlayer(roomId)`)
  - combat room spawners → spawn enemies (`spawnEnemies(roomId)`)
  - portal room spawner → (visual marker only for now)
- The room instance is rendered before the entities.

Multi-room rendering (prototype):

- `Game` builds a floor instance by computing a world position per room (from the grid layout).
- All room instances are rendered each frame.
- There are currently two camera modes:
  - player-follow camera (`sf::View` centered on the player)
  - floor overview camera (zoomed out)

Combat room flow (current prototype):

- `Game::updateActiveRoom()` sets `m_activeRoomId` based on which room bounds contains the player.
- When the player collides with a door trigger in a combat room:
  - a wave is generated (`CombatRoom::generateNewWave(...)`)
  - enemies are spawned from the new spawner list
  - doors are locked for the duration of combat
- `Game::ManageWave()` starts new waves until `m_waveCounter` reaches 0, then clears the room and unlocks doors.

## Likely next steps

- Decide what parts of generation should be deterministic (seed-based) vs “random each run”.
- Add door geometry and room-to-room connections.
- Add collision resolution against wall colliders (right now walls are generated + colliders exist, but movement resolution may still be pending).
