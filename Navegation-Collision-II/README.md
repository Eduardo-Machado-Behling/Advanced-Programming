# Navigation (Grid-Based Collision Avoidance)

This project is a C++ simulation module that implements **grid-based local collision avoidance** for multiple moving agents.
It uses a **velocity-based reservation strategy** on a discrete grid to prevent agents from occupying the same cell at the same time, while still allowing smooth continuous movement in world space.

The system is designed to be lightweight, deterministic, and suitable for real-time simulations or games where many agents navigate simultaneously without global path replanning.

---

## Overview

The navigation logic is split into two main components:

* **GridSystem**
  Manages static obstacles and dynamic agent reservations on a grid.
* **GridStrategy**
  Computes collision-free velocities for agents based on their preferred movement direction.

Instead of computing full paths every frame, the system performs **local prediction + reservation**, making it fast and scalable.

---

## Core Concepts

### Grid Representation

* The world is discretized into grid cells.
* Each cell can be:

  * **Blocked** (static wall)
  * **Free**
  * **Reserved** by an agent for the current simulation step

### Reservation Table

* A simplified **reservation table** maps:

  ```
  CellID → AgentID
  ```
* Reservations are **cleared every frame**, making the system *stateless across steps*.
* This avoids deadlocks and keeps behavior predictable.

---

## Algorithm Summary

### Velocity-Based Reservation Strategy

Each agent follows this pipeline every frame:

1. **Preferred Velocity**

   * Computed externally (e.g., path-following, steering, waypoint logic).
2. **Forward Prediction**

   * Predicts the next position using a short time horizon (e.g. `0.1s`).
3. **Collision Check**

   * Checks if the target grid cell is:

     * A static wall, or
     * Already reserved by another agent.
4. **Decision**

   * **Blocked** → Stop and reserve current cell.
   * **Free** → Reserve next cell and move.

This produces smooth motion while guaranteeing **no two agents occupy the same grid cell**.

---

## Key Classes

### `GridSystem`

Responsible for:

* Static map caching
* Cell occupancy checks
* Reservation handling

**Main Responsibilities**

* Convert world positions to grid cell IDs
* Detect static obstacles
* Prevent dynamic agent collisions

---

### `GridStrategy`

Responsible for:

* Computing the agent’s final velocity
* Interfacing between agent intent and grid constraints

**Key Method**

```cpp
Vec2 computeVelocity(Simulation::Agent* me);
```

This method enforces collision avoidance while preserving the agent’s desired motion as much as possible.

---

## Algorithm & Complexity

### Collision Checking

Each agent performs:

* A constant number of grid lookups
* A single reservation write

#### Time Complexity (per agent, per frame)

* **O(1)** average

  * Hash map lookup for reservations
  * Constant-time wall check

#### Space Complexity

* **O(A)** per frame
  Where `A` is the number of active agents (one reservation per agent).

This makes the system suitable for **hundreds or thousands of agents** in real time.

---

## Design Trade-offs

### ✅ Advantages

* Very fast (no global planning)
* Deterministic and stable
* Easy to debug and visualize
* No deadlocks due to frame-reset reservations

### ⚠ Limitations

* Local only (not globally optimal)
* Agents may wait instead of rerouting
* Requires a higher-level planner for long-distance navigation

This system is intentionally designed to be a **local collision avoidance layer**, not a full pathfinding solution.

---

## Typical Use Case

* Crowd simulation
* RTS-style unit movement
* Game AI navigation
* Multi-agent steering with shared space
* As a **post-processing layer** on top of A*, Dijkstra, or waypoint systems

---

## Summary

> This project implements a **grid-based, velocity-driven collision avoidance system** using a **stateless reservation table**.
> The approach prioritizes performance and simplicity, making it ideal for real-time multi-agent simulations where local interactions dominate over global path optimality.

If you want, next we can:

* Add diagrams (grid + reservations)
* Extend this to multi-step reservations
* Combine it with A* or flow fields
* Formalize it for an academic-style report

Just tell me where you want to take it 🚀
