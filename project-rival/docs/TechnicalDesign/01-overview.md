# 1) Overview

[Back to Technical Design](README.md)

## Project summary

This project is a **2D isometric dungeon crawler roguelike** built in C++ using **SFML**.

The long-term vision includes:

- A **hub world** where the player can upgrade their character using currency earned during runs.
- A seeded, procedurally generated dungeon: `Dungeon → Floors → Rooms`.
  - The *dungeon/floor layout* should be **deterministic** for a given seed.
  - Individual room “dressing” (obstacles/spawns/props) can be **non-deterministic** (random each run).
- Interactable NPCs with a dialogue system powered by a **locally running LLM**, integrated via a separate custom C++ wrapper repo.
- Procedurally generated bosses (modular parts + attacks) optionally enhanced by LLM-generated lore.

## Current status (prototype snapshot)

As of the current prototype, the project has:

- Core SFML game loop (`Game`) with fixed timestep updates.
- Player movement + aiming + projectile shooting.
- Enemies with steering behaviours.
- A minimal collision system (AABB + layer/mask filtering).
- Early “rooms” pipeline: `RoomPlan` (data) → `RoomInstance` (render + wall colliders) → runtime collision checks.

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
