# Context: Step-by-step Steering (Seek) wired into Enemy (SFML / C++)

## Contents
1. [High-level goal](#high-level-goal)
2. [User preferences / working style](#user-preferences--working-style)
3. [Project constraints](#project-constraints)
4. [What exists now (code state)](#what-exists-now-code-state)
5. [Known issues / improvements to consider later](#known-issues--improvements-to-consider-later)
6. [Next steps planned](#next-steps-planned)

---

## High-level goal
Implement a reusable **steering system** for `Enemy` (and later other entities) that supports:
- a steering-enabled agent state (position/velocity/limits)
- steering behaviours (starting with **Seek**)
- ability to **switch behaviours at runtime** (trigger-based)
- later ability to **combine** behaviours (blending/priority)

Initial implementation focuses on correctness and learning (tutorial/exam-prep style), not speed.

---

## User preferences / working style
- **Incremental, tutorial-style** changes.
- Avoid dumping a full solution all at once.
- Explain *why* each variable/function exists and what each line is responsible for.
- Avoid introducing concepts like `inline` / namespaces without explanation.
- User intentionally postponed cleanup like removing `using namespace ...` from headers.

---

## Project constraints
- `Game::update` passes `dt` in **seconds** (`timePerFrame.asSeconds()`).
- Entity updates should treat `dt` as seconds.

---

## What exists now (code state)

### Core steering types (`project-rival/Steering.h`)
- `struct SteeringAgent`
  - `position`, `velocity`, `maxSpeed`, `maxAcceleration`
- `class ISteeringBehaviour`
  - `virtual const Vector2f getSteering(const SteeringAgent& agent) = 0;`
- There is also `struct SteerungOutput` (typo in name) but it’s not currently used.

> Note: `Steering.h` currently has `using namespace sf;` (kept for now).

### Seek behaviour (`project-rival/SeekBehaviour.h/.cpp`)
- `SeekBehaviour` derives from `ISteeringBehaviour`.
- Stores a target (`sb_target`) and exposes `setTarget(...)`.
- `getSteering(agent)`:
  - compute `toTarget = target - agent.position`
  - compute desired velocity toward target at `maxSpeed`
  - steering = desiredVelocity − currentVelocity
  - clamp steering to `maxAcceleration`

> Math helpers (`trig_helper::length/normalise/clampMagnitude`) were previously defined in a header and caused LNK2005. That was fixed by **removing helper definitions from the header** and keeping them in a single translation unit.

### Enemy wiring (`project-rival/Enemy.h/.cpp`)
- `Enemy` owns:
  - `SteeringAgent e_agent`
  - `SeekBehaviour e_seek{ Vector2f(0,0) }`
  - `ISteeringBehaviour* e_currBehaviour{ nullptr }`
- In `Enemy::init()` the default behaviour is set:
  - `e_currBehaviour = &e_seek;`
- `Enemy::setTarget(...)` currently only updates seek’s target:
  - `e_seek.setTarget(target);`
- `Enemy::setBehaviour(ISteeringBehaviour* behaviour)` sets `e_currBehaviour`.
- `Enemy::update(dt)` integrates:
  - `a = behaviour->getSteering(e_agent)`
  - `v += a * dt`
  - clamp `v` to `maxSpeed`
  - `p += v * dt`
  - update shape position

### Game wiring (`project-rival/Game.h/.cpp`)
- `Game` includes `Enemy` and renders it.
- `Enemy` is constructed with a start position, e.g. `Enemy m_enemy{ Vector2f(500.f, 500.f) };`
- Each frame, `Game::update(dt)` sets the enemy target to the player’s current position:
  - `m_enemy.setTarget(m_player.getPosition());`
  - then updates player and enemy.

### Player (`project-rival/player.h/.cpp`)
- `player` exposes `const Vector2f getPosition() const { return p_body.getPosition(); }`

---

## Known issues / improvements to consider later
(Not required to proceed, but tracked for future refactors.)

1. **Behaviour switching vs target updates**
   - `setTarget()` should not switch behaviours.
   - Switching should be done by a separate “decision layer” (FSM/AI triggers).

2. **Multiple behaviours**
   - Need a scalable way to:
     - switch behaviours once per trigger (e.g., Seek → Arrive when close)
     - optionally combine behaviours (blend/priority)

3. **Header `using namespace`**
   - Present in headers (`Steering.h`, historically `SeekBehaviour.h`).
   - Risky long term (pollutes translation units), but user postponed cleanup.

4. **Return type const-by-value**
   - `virtual const Vector2f getSteering(...)` returns const by value.
   - Doesn’t add safety; can be simplified later.

5. **Float equality / epsilon**
   - Seek checks `toTarget == Vector2f(0,0)`.
   - More robust to use an epsilon (distance threshold).

6. **Math helper placement**
   - Keep helper function *definitions* in exactly one `.cpp` (or make them `inline` later after explaining).

---

## Next steps planned

### Step A: Add a second behaviour (recommended: `ArriveBehaviour`)
- Teach “arrive” theory:
  - slowing radius
  - desired speed scales down as you get close
  - clamp acceleration
- Implementation goal:
  - `ArriveBehaviour` derives from `ISteeringBehaviour`
  - has `setTarget(...)`, `slowRadius`, `stopRadius` (or similar)

### Step B: Add decision logic (trigger-based switching)
- Keep updating target every frame.
- Add a small AI layer in `Enemy` (or a separate controller):
  - compute distance to target
  - if distance < threshold: switch to `ArriveBehaviour`
  - else: switch to `SeekBehaviour`
- Ensure switching only occurs when state changes (one-time per trigger).

### Step C: Optional behaviour mixing
- Add a `BlendedBehaviour` (sum weighted forces) or `PriorityBehaviour`.

---

If continuing in a new Copilot instance: start by confirming current file contents for `Steering.h`, `SeekBehaviour.*`, `Enemy.*`, and then proceed with implementing `ArriveBehaviour` + switching logic incrementally.
