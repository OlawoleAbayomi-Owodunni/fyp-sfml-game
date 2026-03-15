# 5) Steering (AI) + OOP concepts

[Back to architecture index](README.md)

`Steering.h` sets up a simple architecture for AI movement behaviours.

## Key types

- `SteeringAgent`
  - describes the thing being controlled (position/velocity + limits)
- `ISteeringBehaviour`
  - an interface-like base class with a pure virtual function:
    - `getSteering(const SteeringAgent& agent)`
  - returns a steering vector (treated as “acceleration” in `Enemy::update(dt)`)

## What this enables (polymorphism)

Later, multiple behaviours that all share the same interface can be created, for example:

- `SeekBehaviour : public ISteeringBehaviour`
- `ArriveBehaviour : public ISteeringBehaviour`

Code can store an `ISteeringBehaviour*` and call `getSteering(...)` without caring which concrete behaviour it is.

## Quick definitions

- **Abstract class**: a class you cannot instantiate directly (often because it has a pure virtual function).
- **Interface (C++ style)**: usually an abstract class that only contains virtual functions.
- **Polymorphism**: treating different derived types through a shared base type.
