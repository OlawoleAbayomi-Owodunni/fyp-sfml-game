# 4) Scope + non-goals

[Back to Technical Design](README.md)

This chapter clarifies what we’re building **now** vs what is explicitly deferred.

## In scope (current / near-term)

- Core gameplay loop (movement, combat, basic enemies).
- Room pipeline:
  - `RoomPlan` data
  - generator(s) for at least combat rooms
  - runtime build (`RoomInstance`) + wall collisions
- Iterating on collision correctness and response.

## Out of scope (for now)

- Full physics engine (broadphase, continuous collision, complex response).
- Networking/multiplayer.
- Fully deterministic per-room decoration (acceptable to be random per run).
- Final UI/UX polish.

## Non-goals (intentional simplifications)

- Avoid over-abstracting early (prototype can keep logic in `Game::update` while validating rules).
- Avoid designing a “final” Entity Component System to move all logic out of update prematurely unless the project truly needs it.
