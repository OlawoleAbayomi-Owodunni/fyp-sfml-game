# 7) High-level runtime flow

[Back to Technical Design](README.md)

This chapter answers: “What happens each frame?”

## Per-frame flow (typical)

At a high level:

1. Poll window events.
2. Read input (`InputManager::update()`).
3. Handle game management input (exit / restart).
4. Update player:
   - movement
   - aiming
   - spawn/update projectiles
5. Update enemies:
   - steering-based movement
   - target updates
6. Resolve collisions:
   - player vs enemies
   - bullets vs enemies
   - player/enemies/bullets vs walls
   - player vs triggers (doors / portals)
	- player vs portal trigger
7. Render:
   - rooms (multiple)
   - player
   - enemies
   - debug overlays (if enabled)

## Notes

- Collision checks currently use AABB intersection + layer/mask filtering.
- Current wall response is simple rollback for player/enemies and bullet destruction.
- Door response is the same as walls when doors are locked (a `DOOR_LAYER` collider exists).
