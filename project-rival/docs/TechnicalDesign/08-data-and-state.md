# 8) Data + state

[Back to Technical Design](README.md)

This chapter describes what state exists, who owns it, and what might need saving/loading.

## Runtime state (prototype)

- `Game` owns:
  - game mode (`HUB` / `DUNGEON`)
  - menu UI state (`MenuUI` screen, button selections, pending menu action)
  - room plans + room instances + room world positions
  - active room id and room-presence flags
  - enemy list
  - projectile list
  - active damage-trigger list
  - pickup list
  - chest list
  - combat state (`m_isInCombat`, `m_waveCounter`)
  - floor plan + floor layout
  - dungeon plan (seed + floor progression/completion)
  - next-floor request flag
  - camera mode state
  - hub shop interaction state (`m_activeShop`) and shop/job-board zone shapes
  - job-board interaction flags (`m_isAtJobBoard`, `m_blockJobBoardInteract`)
  - economy/upgrade state (coins, weapon/player upgrade levels, catalog indices, cosmetic index)
  - quest state manager (`QuestManager`)
  - LLM service state
  - HUD state container (`PlayerHUD`)
- `Player` owns:
  - transform/velocity/previous position
  - aim state + reticle state
  - health/death state (`p_health`, `p_maxHealth`, `p_isDead`)
  - weapon loadout entries (`WeaponInLoadout`) and runtime weapon list + selected weapon id
  - player ammo/speed and rumble state
- `Enemy` (base + derived) owns:
  - transform/target + collision profile
  - health/dead flag (`e_health`, `e_maxHealth`, `e_isDead`)
  - per-type state (steering agent, attack timers, weapon state)

Additional notes:

- `RoomPlan.spawners` are tile-space markers used to spawn runtime entities when rooms are generated.
- `RoomPlan.triggers` are tile-space markers used to build trigger interaction colliders.
- Wall/door collisions currently use rollback by restoring previous entity position.

## Procedural generation state

- Seeded generation:
  - dungeon seed
  - floor seed(s) derived from `dungeonSeed + floorId`
- Non-deterministic elements (currently allowed):
  - room obstacles/spawns using `rand()` for per-run variation

## Persistence (planned)

Likely persistence needs for a roguelike:

- Meta-progression (hub upgrades, unlocked items).
- Run state (seed, current floor/room index) if implementing save mid-run.

## Serialization approach (placeholder)

To be decided later:

- JSON (simple to debug) vs binary (faster/smaller) vs custom.
- Keep save format versioned.
