# Context: Generating commit message(s) from staged Git changes

## Contents
1. [High-level goal](#high-level-goal)
2. [Repo / branch context](#repo--branch-context)
3. [What the user asked for](#what-the-user-asked-for)
4. [What staged changes exist (as provided)](#what-staged-changes-exist-as-provided)
5. [Recommended commit split + messages](#recommended-commit-split--messages)
6. [Git commands used / suggested](#git-commands-used--suggested)

---

## High-level goal
Capture enough information about the **currently staged Git changes** to:
- share them across machines/chat instances
- generate good Git commit(s) (title + body) from the staged diff / staged file contents

---

## Repo / branch context
- Repo root (local): `C:\Users\ayola\Desktop\School\Y4\FYP\fyp-sfml-game`
- Branch: `enemies`
- Remote: `origin` -> `https://github.com/OlawoleAbayomi-Owodunni/fyp-sfml-game`

---

## What the user asked for
The user wanted a way to get:
- a “change log” of staged files (what’s staged)
- the staged diff (patch)
- the staged contents of the changed files

…so they can paste that info into Copilot and have it generate an appropriate commit message (and potentially suggest commit splits).

---

## What staged changes exist (as provided)
The user provided staged summary + name/status (no renames/copies; mostly adds).

### Staged adds (`A`)
Docs:
- `project-rival/docs/Architecture/README.md`
- `project-rival/docs/Architecture/01-project-structure.md`
- `project-rival/docs/Architecture/02-player.md`
- `project-rival/docs/Architecture/03-input.md`
- `project-rival/docs/Architecture/04-enemy.md`
- `project-rival/docs/Architecture/05-steering-oop.md`
- `project-rival/docs/Architecture/06-coding-conventions.md`
- `project-rival/docs/DEVELOPMENT_LOG.md`
- `project-rival/docs/README.md`
- `project-rival/docs/Project/Project Proposal Form.docx`

Code scaffolding:
- `project-rival/project-rival/Steering.h` (defines `SteeringAgent`, `ISteeringBehaviour`, and an output struct)
- `project-rival/project-rival/Enemy.cpp` (empty placeholder)
- `project-rival/project-rival/Enemy.h` (empty placeholder)

### Staged modifications (`M`)
- `project-rival/project-rival/player.cpp`
  - Reads controller via `InputManager::pad().leftStick()`
  - Uses controller input when non-zero; otherwise keyboard WASD fallback
  - Normalizes direction to avoid faster diagonal movement
  - Moves with `p_velocity * dt` where `dt` is seconds
  - Uses right trigger to drive rumble (`setRumble`)

- `project-rival/project-rival/player.h`
  - Includes Windows/XInput headers

- `project-rival/project-rival/project-rival.vcxproj.filters`
  - Visual Studio filter organization updated

---

## Recommended commit split + messages
Suggested splitting into 2 commits (docs vs code), since they are independent concerns.

### Commit 1 (docs)
**Title:** `docs: add architecture overview and development log`

Optional body bullets:
- Add architecture chapter pages (player/input/enemy/steering/conventions)
- Add supervisor-friendly development log
- Add project proposal document

### Commit 2 (code)
**Title:** `feat: add steering interface and controller-driven player movement`

Optional body bullets:
- Add steering agent + behaviour interface scaffolding
- Add placeholder enemy files
- Player: support gamepad left stick input with keyboard fallback; set rumble via triggers
- Update Visual Studio filters

A single-commit alternative was also offered:
- `feat: document architecture and extend player input with gamepad support`

---

## Git commands used / suggested
To collect staged info:
- `git diff --cached --name-status`
- `git diff --cached`
- `git diff --cached --summary -M`

To dump staged contents:
- `git show :path/to/file`

PowerShell helper to dump all staged file contents:
- `git diff --cached --name-only | % { "===== FILE: $_ ====="; git show ":$_"; "" }`

To split commits (example approach):
- `git restore --staged <code files...>` then commit docs
- `git add <code files...>` then commit code

---

## Project note (repo instruction reminder)
Project rule from `./.github/copilot-instructions.md` relevant to recent changes:
- `dt` is expected to be **seconds** (e.g., `timePerFrame.asSeconds()`), and entity updates should use it directly.
