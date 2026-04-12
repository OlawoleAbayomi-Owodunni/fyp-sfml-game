# 7) High-level runtime flow

[Back to Technical Design](README.md)

This chapter answers: “What happens each frame?”

## Per-frame flow (typical)

At a high level:

1. Poll window events (`processEvents`).
2. Read input (`InputManager::update()`).
3. Handle game management input (exit/restart/camera toggle).
4. Prepare aim input coordinates:
   - choose active gameplay view (player/floor)
   - map mouse pixel position into world-space for aiming
5. Branch by game mode:
   - `HUB`: update shop overlap/interaction state (`updateHubShops()`)
   - `DUNGEON`: continue dungeon-combat flow
6. Update active room context (`updateActiveRoom()`).
7. Update player:
   - movement
   - aiming
   - weapon logic (may emit projectiles or melee triggers)
8. Update enemies:
   - set targets
   - run type-specific update paths (`TurretEnemy` projectile path, `GruntEnemy` melee-trigger path)
9. Resolve collisions (`CollisionChecks()`):
   - player vs enemies
   - projectiles vs enemies/player
   - damage triggers vs enemies/player
   - player/enemies/projectiles vs walls/locked doors
   - player vs door/portal triggers
10. Update transient combat objects:
   - update/remove destroyed projectiles
   - update/remove expired damage triggers
11. Manage combat wave logic (`ManageWave()` when in active combat room).
12. Handle floor progression:
   - consume `m_requestNextFloor`
   - advance dungeon plan
   - load next floor or return to hub on completion
13. Poll LLM service:
   - consume async init result
   - consume latest response when complete
14. Update HUD values from player/economy state.
15. Render:
   - choose camera mode (player/floor)
   - render rooms, player, enemies, projectiles, triggers, overlays
   - in hub mode, render hub shop zones + prompt UI
   - switch to default view and render HUD

## Notes

- Collision checks currently use AABB intersection + layer/mask filtering.
- Current wall/door response is rollback for player/enemies and projectile destruction.
- Door triggers in combat rooms can start waves and lock doors.
- Portal triggers require explicit interaction input (`Space` / controller `A`) to start a run (hub) or request floor progression (dungeon).
