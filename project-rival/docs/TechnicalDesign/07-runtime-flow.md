# 7) High-level runtime flow

[Back to Technical Design](README.md)

This chapter answers: “What happens each frame?”

## Per-frame flow (typical)

At a high level:

1. Poll window events (`processEvents`).
2. Read input (`InputManager::update()`).
3. Handle game management input (exit/restart/camera toggle).
4. Update active room context (`updateActiveRoom()`).
5. Update player:
   - movement
   - aiming
   - weapon logic (may emit projectiles or melee triggers)
6. Update enemies:
   - set targets
   - run type-specific update paths (`TurretEnemy` projectile path, `GruntEnemy` melee-trigger path)
7. Resolve collisions (`CollisionChecks()`):
   - player vs enemies
   - projectiles vs enemies/player
   - damage triggers vs enemies/player
   - player/enemies/projectiles vs walls/locked doors
   - player vs door/portal triggers
8. Update transient combat objects:
   - update/remove destroyed projectiles
   - update/remove expired damage triggers
9. Manage combat wave logic (`ManageWave()` when in active combat room).
10. Handle floor progression:
   - consume `m_requestNextFloor`
   - advance dungeon plan
   - load next floor or end run (temporary close on completion)
11. Poll LLM service:
   - consume async init result
   - consume latest response when complete
12. Render:
   - choose camera mode (player/floor)
   - render rooms, player, enemies, projectiles, triggers, overlays

## Notes

- Collision checks currently use AABB intersection + layer/mask filtering.
- Current wall/door response is rollback for player/enemies and projectile destruction.
- Door triggers in combat rooms can start waves and lock doors.
- Portal triggers require explicit interaction input (`Space` / controller `A`) to request floor progression.
