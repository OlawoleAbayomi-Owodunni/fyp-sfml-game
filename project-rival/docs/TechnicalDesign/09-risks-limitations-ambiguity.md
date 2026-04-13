# 9) Risks, limitations + ambiguity

[Back to Technical Design](README.md)

This chapter documents known shortcuts and future technical risks.

## Known prototype shortcuts

- Collision response is mostly rollback (set previous position) rather than sliding.
- Collision checks are simple AABB intersections (no broadphase).
- Some generation uses `rand()` directly (non-deterministic room details by design for now).
- Room generation still does not fully use `RoomPlan.seed` for deterministic local outcomes.
- Door placement depends on floor-layout direction inference, and corridor generation may need additional edge-case handling.
- Dungeon completion behavior is still temporary (flow currently returns directly to hub mode, without a dedicated run-results/end screen).
- Active-room detection is simple bounds checking against room rectangles.
- Corridor generation is procedural and may need extra handling for alignment/overlap edge cases.
- Hub-world interactions (shops, job board, NPC dialogue) are currently overlap-driven and centralized in `Game`, so scalability/refactor pressure is expected as systems grow.
- LLM integration is async and queue-driven in `Game`; text is consumed as complete responses (no streaming output), and there is no formal latency budget policy yet.

## Technical risks

- Performance risk if entity counts scale without broadphase/spatial partitioning.
- Complexity risk as room types/interactions and combat systems expand.
- Refactor risk when moving from debug LLM flow to production dialogue systems.

## Ambiguities / decisions still open

- Final architecture style (OOP vs ECS-like patterns).
- How deterministic room appearance/spawn behavior should be long-term.
- How boss generation presets and constraints should be represented in data.
- End-of-run transition behavior after final floor completion (menu/hub/results).


