# 9) Risks, limitations + ambiguity

[Back to Technical Design](README.md)

This chapter documents known shortcuts and future technical risks.

## Known prototype shortcuts

- Collision response is mostly “rollback” (set old position) rather than slide.
- Collision checks are simple AABB intersections (no broadphase).
- Player owns bullets (may move to a world/weapon system later).
- Some generation uses `rand()` directly (non-deterministic by design for room details).
- Room generation currently does not use `seed` to drive deterministic outcomes (even though `RoomPlan` stores a seed).
- Door placement currently depends on the generated floor layout (direction inferred from grid positions) and corridor generation assumes doors are aligned well enough to connect.
- Portal interaction currently advances floors, but completion behavior is temporary (on dungeon complete, the game exits instead of returning to a menu/hub).
- Active-room detection is a simple bounds check against each room’s rectangle (no spatial indexing).
- Corridor generation is procedural and may need extra handling for edge cases (misaligned door spans, overlapping corridors, or collisions near door thresholds).

## Technical risks

- Performance risk if entity counts scale up without broadphase collision.
- Complexity risk as more room types and interactables are added.
- Refactor risk when integrating the LLM wrapper and NPC dialogue system.

## Ambiguities / decisions still open

- Final architecture style (OOP vs ECS-like patterns).
- How deterministic “room appearance” should be.
- How boss generation presets and constraints should be represented in data.

## Near-term follow-ups (not blockers)

- Decide whether to keep rollback collisions or implement axis-separated sliding for walls.
- Decide whether enemy-wall response should differ per enemy type (bounce/slide/stop).
