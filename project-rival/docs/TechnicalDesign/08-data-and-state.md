# 8) Data + state

[Back to Technical Design](README.md)

This chapter describes what state exists, who owns it, and what might need saving/loading.

## Runtime state (prototype)

- `Game`:
  - active room plan + room instance
  - enemy list
- `Player`:
  - transform/velocity
  - current projectiles
  - health
- `Enemy`:
  - health/dead flag
  - steering state

## Procedural generation state

- Seeded generation (planned):
  - dungeon seed
  - floor seed(s) derived from dungeon seed + floor index
- Non-deterministic elements (allowed):
  - room obstacles and spawns using `rand()` for variety

## Persistence (planned)

Likely persistence needs for a roguelike:

- Meta-progression (hub upgrades, unlocked items).
- Run state (seed, current floor/room index) if implementing save mid-run.

## Serialization approach (placeholder)

To be decided later:

- JSON (simple to debug) vs binary (faster/smaller) vs custom.
- Keep save format versioned.
