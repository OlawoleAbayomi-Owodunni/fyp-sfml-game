# 3) Technical requirements

[Back to Technical Design](README.md)

## Platform + tooling

- Language: C++.
- Graphics/input: SFML.
- IDE/tooling: Visual Studio project.
- OS target (current): Windows.
- Controller input (current): XInput (via a `Gamepad` wrapper).

## Constraints

- Collision filtering uses **layers + masks** (bitmask filtering).
- Prototype-first approach: simple systems that scale into advanced systems over time.
- The game loop passes `dt` in **seconds** (fixed timestep ~60 FPS).

Procedural generation split (current intent):

- Floor graph/layout generation is deterministic (uses `std::mt19937` seeded with `dungeonSeed + floorId`).
- Room details (obstacles, spawns) use `rand()` for per-run variety.

## Assumptions

- LLM dialogue uses a locally running model, accessed through a custom C++ wrapper yet to be integrated (separate repo).
- Room “layout determinism” and “room appearance randomness” are both acceptable (seeded vs non-seeded split).

## External dependencies (planned)

- LLM wrapper repo (to be integrated as a submodule/subtree or linked library later).
