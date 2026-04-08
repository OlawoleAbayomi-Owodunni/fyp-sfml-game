# Context: Level architecture (Dungeon/Floor/Room) + RoomPlan/Generator/Builder + static collider tiles (SFML / C++)

## Contents
1. [High-level goal](#high-level-goal)
2. [User preferences / working style](#user-preferences--working-style)
3. [Current codebase state (relevant pieces)](#current-codebase-state-relevant-pieces)
4. [Design decisions reached in this chat](#design-decisions-reached-in-this-chat)
5. [Incremental implementation plan (agreed)](#incremental-implementation-plan-agreed)
6. [What is already done](#what-is-already-done)
7. [Next step to implement](#next-step-to-implement)

---

## High-level goal
Implement a roguelike-style procedural architecture:

- `Dungeon -> Floors -> Rooms`
- Floors are generated as graphs (nodes=rooms, edges=connections), then embedded into 2D layout.
- Rooms are built from tile-data (walls/floors/doors) and spawn markers.
- Static room geometry (walls/obstacles) should support collision for player/enemies.

Immediate objective: get a minimal room pipeline working end-to-end:

1. Create a `RoomPlan` (blueprint data)
2. Generate a test plan (hard-coded first)
3. Build runtime wall colliders from tiles
4. Resolve player vs wall collisions

---

## User preferences / working style
- Prefer **incremental, buildable steps** (tutorial style) with some coding theory sprinkled in to justify decisions.
- Each step should compile/run and be clearly an extension of the previous.
- Avoid introducing advanced concepts without explanation.
- User cares about **consistency** between architectural advice and code examples.

---

## Current codebase state (relevant pieces)

### Game loop
- `Game::update(double dt)` runs at fixed ~60 FPS and passes `dt` in **seconds**.
- `Game` currently updates:
  - `Player` (movement + aiming + bullets)
  - `Enemy` instances (grunt steering + turret placeholder)
  - collision checks: `Player vs Enemy`, `Player bullets vs Enemy`

### Collision system
- `ICollidable` exposes:
  - `sf::FloatRect getCollisionBounds() const`
  - `CollisionProfile getCollisionProfile() const`
- `CollisionCheck::areColliding(a,b)`:
  - checks layer/mask compatibility
  - checks bounds intersection

### Entities
- `Player` and `Enemy` derive from `ICollidable`.
- Collision filtering uses layers/masks.

---

## Design decisions reached in this chat

### Floor/room connectivity
- Floor topology should be represented as a **graph**.
  - Node = room descriptor
  - Edge = connection/door between rooms
- Adjacency structure is recommended for neighbor queries.
  - `unordered_map<roomId, vector<neighborId>>` is flexible for sparse IDs.
  - `vector<vector<int>>` is simpler/faster if room IDs are dense `0..N-1`.

### Level elements / tiles vs objects
- Static geometry (walls/floors/most obstacles) should be represented as **tile data** in a room grid.
- Runtime collision should be built from that data as a list of **static colliders**.
  - Keep it lightweight (no per-tile AI/update).
- Stateful/interactable objects (barrels, chests, shrines) should be spawned as normal game objects.

### Room pipeline (preferred)
- Use a 3-stage pipeline:
  1. `RoomPlan` (blueprint/data)
  2. `IRoomGenerator` (algorithm that fills a `RoomPlan` based on `RoomType`)
  3. `RoomBuilder` (converts `RoomPlan` into runtime representation: wall colliders + visuals + spawns)

---

## Incremental implementation plan (agreed)
1. Add collision layer for walls (`WALL_LAYER`).
2. Add `RoomPlan` types (tiles/doors/markers) with no runtime wiring.
3. Add generator interface + one hard-coded generator (e.g., combat room).
4. Add builder that produces:
   - `std::vector<sf::FloatRect>` solid wall colliders (from wall tiles)
   - optional debug visuals for walls/floor
5. Wire into `Game`:
   - build a test room in `gameStart()`
   - in `update()`, resolve `Player` vs room solids
   - in `render()`, draw room debug visuals
6. Improve collision response from "rollback" to "axis-separated slide".

---

## What is already done
- Step 1 completed: `project-rival/Collision.h` updated to include:
  - `WALL_LAYER = 1 << 4`
  - Extra grouping comments for entity/static environment layers.

No runtime behavior changes were made yet.

---

## Next step to implement
**Step 2:** create `RoomPlan` (header-only for now) and add it to the Visual Studio project.

Suggested file:
- `project-rival/RoomPlan.h`
  - `RoomType`, `TileType`
  - door definition(s) (tile position + direction + locked)
  - spawn markers (tile-space)
  - `RoomPlan` with `width/height/tileSize/seed`, `tiles` (1D), helper `getTile`/`setTile`, and `isValid()`

After Step 2 builds, proceed to Step 3 (hard-coded generator).
