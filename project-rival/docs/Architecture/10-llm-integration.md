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
  - `initAsync(modelPath)` → begins loading a model on a worker thread.
  - `tryConsumeInitResult()` → polls for async init completion.
  - `isReady()` → reports status.
  - `requestGenerate(prompt)` → queues a generation request on a worker thread.
  - `tryConsumeLatestResponse()` → polls for the most recent completed response.

The game currently uses the async flow (init + request + poll) rather than calling `generateResponse(prompt)` directly.

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

- Generation is currently asynchronous in the game loop, but responses are consumed as whole strings (no token streaming to UI).
- No conversation state/history handling exists yet.
- No gameplay integration exists yet (no NPC entities, no UI layer for dialogue, no prompt templates).

## Performance notes (local benchmarks)

The following timings were manually recorded during local testing. Format: bootup time = first-time model load; inference times = end-to-end time per request (single prompt).

| Build | Bootup (model load) | Inference samples | Average inference time |
| --- | --- | --- | --- |
| x64 Debug | 15.46 s | 00:30.23, 00:29.21, 00:30.66 | 00:30.03 |
| x64 Release | 2.23 s | 00:05.51, 00:05.38, 00:05.58 | 00:05.49 |
| x86 Debug | 33.12 s | 1:07.11, 1:08.64, 1:08.69 | 1:08.15 |
| x86 Release | 1.25 s | 00:10.87, 00:09.70, 00:09.32 | 00:09.96 |

Notes:

- These numbers will vary by machine, OS power settings, and model.
- Bootup time is sensitive to disk and model caching.

## Next likely steps

- Add an NPC dialogue layer that builds prompts from:
  - NPC identity + role
  - player state
  - current room/floor context
  - conversation history
- Add response constraints/formatting (e.g., JSON-like tool outputs, safe-length limits).
- Consider latency mitigation (caching, background worker thread, streaming output) if required.
