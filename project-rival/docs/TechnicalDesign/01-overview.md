# 1) Overview

[Back to Technical Design](README.md)

## Project summary

This project is a **2D isometric dungeon crawler roguelike** built in C++ using **SFML**.

The long-term vision includes:

- A **hub world** where the player can upgrade their character using currency earned during runs.
- A seeded, procedurally generated dungeon: `Dungeon → Floors → Rooms`.
  - The *dungeon/floor layout* should be **deterministic** for a given seed.
  - Individual room “dressing” (obstacles/spawns/props) can be **non-deterministic** (random each run).
- Interactable NPCs with a dialogue system powered by a **locally running LLM**, integrated via a vendored custom C++ wrapper.
- Procedurally generated bosses (modular parts + attacks) optionally enhanced by LLM-generated lore.

## Current status (prototype snapshot)

As of the current prototype, the project has:

- Core SFML game loop (`Game`) with fixed timestep updates.
- Two runtime modes: `HUB` and `DUNGEON`.
- `MenuUI` screen flow for main menu, pause menu, gameplay, and game-over screen.
- Player movement + aiming + weapon switching.
- Player loadout/economy hooks (weapon swapping and upgrade application in hub flow).
- Ranged projectiles and melee damage triggers via weapon classes.
- Enemies with steering behaviours and turret projectile attacks.
- A minimal collision system (AABB + layer/mask filtering).
- Rooms pipeline: `RoomPlan` (data) → `RoomInstance` (render + static colliders) → runtime collision checks.
- Floor pipeline (prototype):
  - `FloorPlan` graph generation (`FloorGenerator`)
  - simple 2D embedding (`FloorLayoutGenerator`)
  - doors applied from graph edges (`RoomDoorUtils`)
  - corridors generated at runtime to connect doorways
- Multi-room rendering with two camera modes:
  - player-follow camera
  - zoomed-out floor overview (toggle)
- Early combat wave logic for combat rooms (door trigger starts combat, waves spawn enemies, doors lock/unlock).
- Multi-floor progression (prototype): portal interaction can advance to the next floor; when the dungeon is complete, flow currently returns to hub.
- Prototype LLM async integration with debug prompt triggers and polled console output.
- On-screen `PlayerHUD` overlay for health, ammo, and coin display.

## Core / key terminology

- **Layer + mask collision filtering**:
  - “Layer” = what an object *is* (player, enemy, wall, bullet).
  - “Mask” = what an object *can collide with*.
- `ICollidable`: capability interface for anything that participates in collision checks.
- `RoomPlan`: a lightweight blueprint describing room tiles and spawns.
- `RoomInstance`: runtime representation created from a `RoomPlan` (shapes + static colliders).
- “Seeded procedural generation”:
  - a repeatable algorithm that produces the same high-level layout for the same seed.

## Links

- Architecture docs: `../Architecture/README.md`
- Development log: `../DEVELOPMENT_LOG.md`
