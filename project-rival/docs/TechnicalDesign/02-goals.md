# 2) Goals

[Back to Technical Design](README.md)

This chapter defines what the system must achieve to be considered “complete”.

## Must-have goals (definition of done)

- Playable core loop:
  - Player can move, aim, and attack.
  - Enemies spawn and behave consistently.
  - Collisions and damage feel reliable.
- Procedural dungeon structure:
  - Generate `Floors → Rooms` from a seed.
  - Support core room types (at minimum: spawn, combat, portal).
- Upgrade loop:
  - Hub world exists.
  - Currency is earned during runs.
  - Player can spend currency on upgrades in the hub.
- NPC interaction:
  - NPCs can be interacted with.
  - Dialogue can be driven by a local LLM integration.

## Stretch / milestone goals

- More room types (shop, treasure, miniboss, boss).
- More weapons (melee + ranged, primary/secondary).
- Procedural boss generation (modular body + attacks) with lore integration.
- Save/load of meta-progression (hub upgrades, unlocked items).

## Quality goals

- Readable architecture (clear ownership, minimal coupling).
- Small, testable subsystems (generation, collision, combat).
- Performance: stable framerate at target resolution with typical room sizes.
