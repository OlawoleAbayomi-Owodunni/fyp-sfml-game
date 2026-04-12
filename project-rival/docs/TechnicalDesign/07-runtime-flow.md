# 7) High-level runtime flow

[Back to Technical Design](README.md)

This chapter answers: “What happens each frame?”

## Per-frame flow (typical)

At a high level:

1. Poll window events (`processEvents`).
2. For each event, route through game event handling:
   - if menu is active, process menu input and consume menu actions
   - otherwise process gameplay key events
3. Read input (`InputManager::update()`).
4. Handle controller-level controls (`ControllerInputHandler()`):
   - pause/menu toggling
   - gameplay controls when gameplay screen is active
   - menu navigation/actions when menu screen is active
5. If menu is not on gameplay screen, return early from update.
6. Prepare aim input coordinates:
   - choose active gameplay view (player/floor)
   - map mouse pixel position into world-space for aiming
7. If player dies in dungeon mode:
   - finalize quest run state
   - set menu to `GAME_OVER` screen
   - return from update
8. Update active room context (`updateActiveRoom()`).
9. Update player:
   - movement
   - aiming
   - weapon logic (may emit projectiles or melee triggers)
10. Update enemies:
    - set targets
    - run type-specific update paths (`TurretEnemy` projectile path, `GruntEnemy` melee-trigger path)
    - on enemy death: record quest kill progress, roll pickup spawn, remove enemy
11. Update/remove transient combat objects:
    - update/remove destroyed projectiles
    - update/remove expired damage triggers
12. Resolve collisions (`CollisionChecks()`):
    - player vs enemies
    - projectiles vs enemies/player
    - damage triggers vs enemies/player
    - pickups/chests vs player
    - NPC interaction overlaps vs player
    - player/enemies/projectiles vs walls/locked doors
    - player vs door/portal triggers
13. Remove destroyed pickups from runtime list.
14. Manage combat wave logic (`ManageWave()` when in active combat room).
15. Handle floor progression:
    - consume `m_requestNextFloor`
    - advance dungeon plan
    - load next floor or return to hub on completion
    - when returning to hub, commit quest run result/reward
16. Poll LLM service:
    - consume async init result
    - consume latest response when complete
17. Process hub interactions (when in `HUB` mode):
    - shops
    - job board / quest board opening
    - NPC dialogue interactions
18. Update HUD values from player/economy/quest state.
19. Render:
    - choose camera mode (player/floor)
    - render rooms, player, enemies, projectiles, triggers, pickups, chests, hub NPCs, overlays
    - in hub mode, render shop/job-board prompts and NPC dialogue prompts
    - switch to default view
    - render HUD on gameplay screen, otherwise render menu UI

## Notes

- Collision checks currently use AABB intersection + layer/mask filtering.
- Current wall/door response is rollback for player/enemies and projectile destruction.
- Door triggers in combat rooms can start waves and lock doors.
- Portal triggers require explicit interaction input (`Space` / controller `A`) to start a run (hub) or request floor progression (dungeon).
