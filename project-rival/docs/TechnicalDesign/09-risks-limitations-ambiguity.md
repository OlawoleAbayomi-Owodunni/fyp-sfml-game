# 9) Risks, limitations + ambiguity

[Back to Technical Design](README.md)

This chapter documents known shortcuts and future technical risks.

## Known prototype shortcuts

- Collision response is mostly rollback (set previous position) rather than sliding.
- Collision checks are simple AABB intersections (no broadphase).
- Some generation uses `rand()` directly (non-deterministic room details by design for now).
- Room generation still does not fully use `RoomPlan.seed` for deterministic local outcomes.
- Door placement depends on floor-layout direction inference, and corridor generation may need additional edge-case handling.
- Portal progression completion behavior is temporary (dungeon completion currently closes the game window).
- Active-room detection is simple bounds checking against room rectangles.
- Corridor generation is procedural and may need extra handling for alignment/overlap edge cases.
- LLM integration is async and service-level/debug-trigger only; there is no streaming output, no gameplay-level NPC dialogue system, and no formal latency budget policy yet.

## Technical risks

- Performance risk if entity counts scale without broadphase/spatial partitioning.
- Complexity risk as room types/interactions and combat systems expand.
- Refactor risk when moving from debug LLM flow to production dialogue systems.

## Ambiguities / decisions still open

- Final architecture style (OOP vs ECS-like patterns).
- How deterministic room appearance/spawn behavior should be long-term.
- How boss generation presets and constraints should be represented in data.
- End-of-run transition behavior after final floor completion (menu/hub/results).


