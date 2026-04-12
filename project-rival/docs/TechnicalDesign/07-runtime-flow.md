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
7. Branch by game mode:
   - `HUB`: update shop overlap/interaction state (`updateHubShops()`)
   - `DUNGEON`: continue dungeon-combat flow
8. If player dies in dungeon mode:
   - set menu to `GAME_OVER` screen
   - return from update
9. Update active room context (`updateActiveRoom()`).
10. Update player:
    - movement
    - aiming
    - weapon logic (may emit projectiles or melee triggers)
11. Update enemies:
    - set targets
    - run type-specific update paths (`TurretEnemy` projectile path, `GruntEnemy` melee-trigger path)
12. Resolve collisions (`CollisionChecks()`):
    - player vs enemies
    - projectiles vs enemies/player
    - damage triggers vs enemies/player
    - player/enemies/projectiles vs walls/locked doors
    - player vs door/portal triggers
13. Update transient combat objects:
    - update/remove destroyed projectiles
    - update/remove expired damage triggers
14. Manage combat wave logic (`ManageWave()` when in active combat room).
15. Handle floor progression:
    - consume `m_requestNextFloor`
    - advance dungeon plan
    - load next floor or return to hub on completion
16. Poll LLM service:
    - consume async init result
    - consume latest response when complete
17. Update HUD values from player/economy state.
18. Render:
    - choose camera mode (player/floor)
    - render rooms, player, enemies, projectiles, triggers, overlays
    - in hub mode, render hub shop zones + prompt UI
    - switch to default view
    - render HUD on gameplay screen, otherwise render menu UI

## Notes

- Collision checks currently use AABB intersection + layer/mask filtering.
- Current wall/door response is rollback for player/enemies and projectile destruction.
- Door triggers in combat rooms can start waves and lock doors.
- Portal triggers require explicit interaction input (`Space` / controller `A`) to start a run (hub) or request floor progression (dungeon).
