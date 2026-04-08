# 10) LLM integration (prototype)

[Back to architecture index](README.md)

This document describes the current **local LLM integration** in the solution.

## Purpose

The goal is to support future NPC dialogue and lore generation using a locally-run model.

At the moment, the integration is **service-level only** (load model + generate a string). There is not yet a gameplay-level NPC dialogue system.

## Key components

### `LLMService`

Files:

- `project-rival/LLMService.h`
- `project-rival/LLMService.cpp`

Responsibilities:

- Owns an `LLMWrapper` instance.
- Tracks readiness (`llm_isReady`).
- Provides a minimal API:
  - `init(modelPath)` → loads a model.
  - `isReady()` → reports status.
  - `generateResponse(prompt)` → returns generated text (or an error string if not ready).

### `LLMWrapper` (vendored dependency)

Files:

- `external/fyp-llm-lib/llm/LLMWrapper.h`

Notes:

- Wraps `llama.cpp` resources (`llama_model`, `llama_context`, sampler).
- Exposes:
  - `LoadModel(model_path)`
  - `UnloadModel()`
  - `Generate(prompt)`

## Model assets

A `.gguf` model is currently present under:

- `project-rival/ASSETS/LLM/MODELS/`

## Current limitations

- The API is synchronous and returns the full response as a string.
- No conversation state/history handling exists yet.
- No gameplay integration exists yet (no NPC entities, no UI layer for dialogue, no prompt templates).

## Next likely steps

- Add an NPC dialogue layer that builds prompts from:
  - NPC identity + role
  - player state
  - current room/floor context
  - conversation history
- Add response constraints/formatting (e.g., JSON-like tool outputs, safe-length limits).
- Consider latency mitigation (caching, background worker thread, streaming output) if required.
