# Context: Docs split (architecture chapters) + development log

## Contents
1. [High-level goal](#high-level-goal)
2. [User preferences / working style](#user-preferences--working-style)
3. [What was done in this chat](#what-was-done-in-this-chat)
4. [Current documentation layout](#current-documentation-layout)
5. [How to update the development log going forward](#how-to-update-the-development-log-going-forward)
6. [Next steps planned](#next-steps-planned)

---

## High-level goal
Keep project documentation **simple, short, and readable**:

- Architecture docs should be easy to navigate even as the codebase grows (chapter-like).
- Maintain a supervisor-friendly **timeline / development log** (weekly or milestone based).

---

## User preferences / working style
- Keep docs beginner-friendly and **not unnecessarily long**.
- Architecture documentation should avoid going deep on the game loop.
- `InputManager` / `Gamepad` should be described only at a high level.
- Prefer a “book” style for docs (index page + smaller pages/chapters).
- Development log should be **weekly** most of the time, but can be **milestone-based** if a big feature lands quickly.
- User will add “why/intent” in future updates; Copilot should help append/format.

---

## What was done in this chat

1) Split architecture documentation into multiple small Markdown files under `docs/architecture/`.

2) Added a documentation index at `docs/README.md`.

3) Created a timeline document at `docs/DEVELOPMENT_LOG.md` using git history provided by the user.

4) Updated `docs/README.md` so it links to:
- `docs/architecture/README.md`
- `docs/DEVELOPMENT_LOG.md`

---

## Current documentation layout

### Docs index
- `docs/README.md`
  - Links to architecture index + development log
  - Links to proposal doc

### Architecture (chapter-style)
- `docs/architecture/README.md` (index + clickable TOC)
- `docs/architecture/01-project-structure.md`
- `docs/architecture/02-player.md`
- `docs/architecture/03-input.md`
- `docs/architecture/04-enemy.md`
- `docs/architecture/05-steering-oop.md`
- `docs/architecture/06-coding-conventions.md`

### Development timeline
- `docs/DEVELOPMENT_LOG.md`
  - Filled with sections for:
    - Week of 2026-01-22 (controller support + omni movement)
    - Week of 2026-01-21 (player wired into game + movement iteration)
    - Week of 2025-11-24 (initial setup)
  - Includes a copy/paste template for new entries.

### Proposal doc (source material for README)
- `docs/Project/Project Proposal Form.docx`

---

## How to update the development log going forward
Best workflow:

1) Get new commits since last update:

- `git log --date=short --name-only --pretty=format:"%ad %h %s" <lastHash>..HEAD`

2) Provide 2–5 bullets of “Why / intent” for that week/milestone.

3) Append a new section to `docs/DEVELOPMENT_LOG.md`:
- `Week of YYYY-MM-DD` OR `Milestone: Name`
- Summary / Technical notes / Evidence / Why

---

## Next steps planned
- Create the project root `README.md` using the proposal doc (`docs/Project/Project Proposal Form.docx`) as source.
- Optionally add a milestone-based entry when the steering/AI work (Seek/Enemy wiring) lands.
