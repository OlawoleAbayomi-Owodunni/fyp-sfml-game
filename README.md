# Space Raider (Final Year Capstone SFML Game project)

A top-down action game prototype built with SFML, with procedural room/floor generation, enemy combat, hub interactions, and local LLM-powered text generation (NPC replies, room insights, and quest text) using a custom llm wrapper (https://github.com/OlawoleAbayomi-Owodunni/fyp-llm-library).

## Repository structure

- `project-rival/` - main game source, assets, and Visual Studio project
- `external/fyp-llm-lib/` - git submodule for local `llama.cpp` wrapper (`LLMLib`)
- `build/` - generated build output (including prebuilt LLM dependency outputs)
- `docs/` - generated Doxygen output (HTML)
- `Project Documents/` - Report, Game and Technical Design documentation

## Core features

- Enemy types and steering-based movement behavior
- Room generation and multi-room floor layout
- NPC Chat system powered via the local model
- Quest and Room context text metadata generation using a local model
- Asynchronous LLM job queue integration in runtime

## Tech stack

- C++17
- SFML
- CMake presets for LLM dependency builds
- Local inference library: `LLMLib` (from `external/fyp-llm-lib`)

## Prerequisites (Windows)

- Visual Studio 2026 (or compatible MSVC toolset configured for this solution)
- CMake 4.x+
- Git (with submodule support)
- SFML installed locally
- A GGUF model file for the local LLM path used by the game

## Install Insturctions
### 1) Clone with submodules

```powershell
git clone --recurse-submodules <THIS_REPO_URL>
cd fyp-sfml-game
```

If already cloned:

```powershell
git submodule update --init --recursive
```

### 2) Build LLM dependency libraries

The game links against `LLMLib`, `llama`, and `ggml*` static libs produced via CMake presets in this repo.

Example (x64 Debug):

```powershell
cmake --preset llm_x64_debug
cmake --build --preset llm_x64_debug
```

Other available presets:

- `llm_win32_debug`
- `llm_win32_release`
- `llm_x64_debug`
- `llm_x64_release`

### 3) Configure SFML include/lib paths

The Visual Studio project uses environment variables for SFML paths:

- `SFML_SDK` (Win32)
- `SFML_SDK_x64` (x64)

Each should point to the root of the matching SFML SDK install (the folder that contains `include/` and `lib/`).

### 4) Ensure model file exists at runtime path

The repo does not include the model to load.

Within `external/fyp-llm-lib` The provided script in `resources/scripts/` downloads to `./downloaded_resources` relative to your *current working directory*, so run it from `resources/`:

```powershell
cd resources
python .\scripts\download_model.py
cd ..
```

If Hugging Face requires authentication for the model repo, log in first (one-time) with `huggingface-cli login` or set `HUGGINGFACE_HUB_TOKEN`.

After create a folder `LLM/MODELS` within `ASSETS` and move the model to that directory

At startup, the game loads this model path:

- `project-rival/ASSETS/LLM/MODELS/Llama-3.2-1B-Instruct-Q4_K_M.gguf`

If the file is missing, LLM-backed features will not initialize.

## 5) Build and run the game

1. Open `project-rival.sln` in Visual Studio.
2. Select platform/configuration (for example: `x64` + `Debug`).
3. Build solution.
4. Set `project-rival` as startup project if needed.
5. Run.

## Controls (current prototype)

Keyboard + mouse:

- Move: `W A S D`
- Aim: mouse position
- Fire: left mouse button
- Interact/confirm: `Space`
- Secondary confirm in some menus: `Enter`
- Pause/back: `Escape`
- Restart run: `R`
- Toggle camera mode: `V`
- Room insight prompt: `0` (Num0)

Gamepad:

- Move: left stick
- Aim: right stick
- Fire: right trigger
- Switch weapon: left/right bumper
- Interact/confirm: `A`
- Back/cancel: `B`
- Pause: `Start`
- Restart run: `Select`
- Room insight prompt: `DPad Up`

## Documentation

Internal docs are under `project-rival/docs/`:

- `README.md` - docs index
- `Architecture/` - subsystem-level architecture notes
- `TechnicalDesign/` - structured technical design chapters
- `DEVELOPMENT_LOG.md` - milestone/week progress log

## Notes and limitations

- LLM generation is asynchronous and consumed as full responses (no streaming UI yet).
- Runtime behavior and balance are still under active iteration.
- This repository currently targets Windows-first development workflow.

## Troubleshooting

- Linker errors for `LLMLib`/`llama`/`ggml*`: build the matching LLM preset for your selected platform/configuration.
- SFML headers/libs not found: verify `SFML_SDK` / `SFML_SDK_x64` and restart Visual Studio after setting variables.
- LLM features not responding: verify model file exists at the expected path and that initialization completed.
