# 7) High-level runtime flow

[Back to Technical Design](README.md)

This chapter answers: “What happens each frame?”

## Per-frame flow (typical)

At a high level:

1. Poll window events.
2. Read input (`InputManager::update()`).
3. Update player:
   - movement
   - aiming
   - spawn/update projectiles
4. Update enemies:
   - steering-based movement
   - target updates
5. Resolve collisions:
   - player vs enemies
   - bullets vs enemies
   - objects vs walls
6. Render:
   - room
   - player
   - enemies
   - debug overlays (if enabled)

## Notes

- Collision checks currently use AABB intersection + layer/mask filtering.
