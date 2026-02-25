# 6) Coding / design conventions

[Back to architecture index](README.md)

This is the current style used in the codebase.

## File layout

- Headers (`.h`) declare types/functions
- Source files (`.cpp`) implement them

## `dt` (delta time)

- `dt` is passed around as **seconds**.
- Entity movement uses `velocity * dt`.

## Update/render separation

Most gameplay classes follow a simple pattern:

- `update(dt)` changes state
- `render(...)` draws the current state
