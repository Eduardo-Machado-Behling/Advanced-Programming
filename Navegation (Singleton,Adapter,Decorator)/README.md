# Navegation (Singleton, Adapter, Decorator)

This project is an interactive C++/GLFW application that implements a grid-based navigation algorithm. It uses **Dijkstra's algorithm** to find and visualize the shortest path between multiple start and end points on a 2D grid populated with obstacles.

The application focuses on implementing advanced Design Patterns to structure the application logic, rendering, and graph topology.

## Changes

This iteration of the project introduces three specific Design Patterns as required by the assignment:

- **Singleton**: Applied to the `GridManager` and `CellFactory`. This ensures that the grid state and the cell creation logic are centralized and accessible globally without passing pointers, guaranteeing only one instance of the grid management system exists during runtime.
- **Adapter**: Implemented `HexagonalGridAdapter` and `SquareGridAdapter`. These classes adapt the specific geometry of square (4 neighbors) and hexagonal (6 neighbors) grids into a common `IGraph` interface. This allows the Dijkstra algorithm to calculate paths on *any* grid topology without knowing the underlying shape.
- **Decorator**: Implemented `CellDecorator` and `PulsingDecorator`. This pattern is used to dynamically add behaviors to entities in real-time. For example, `Obstacle` cells are now wrapped in a `PulsingDecorator` that adds a visual "pulsing" animation on top of the base obstacle drawing logic without modifying the original `Obstacle` class.

## Interactive Controls

* **Define Grid Area:** `Left-Click` and drag to define the bounding box of the grid.
    * **Console Prompt:** After releasing, look at the terminal to input:
        1. Number of Rows
        2. Number of Columns
        3. Grid Type (`1` for Square, `2` for Hexagonal)

* **Place Obstacle:**
    * `Left-Click (Hold)` and drag over grid cells.
    * OR, press `F2` while hovering over a cell.
    * *Note:* Obstacles now feature a real-time pulsing effect via the Decorator pattern.

* **Define Start/End Pair:** `F1 (Hold)` over the start cell, drag to the end cell, and `F1 (Release)`.

* **Clear Cell:** `Right-Click (Hold)` and drag over an obstacle, start, or end cell to clear it.

* **Run Pathfinding:** Press `SPACE` to run Dijkstra's algorithm for all defined start/end pairs. The paths will be animated.

## 📈 Algorithm & Complexity

The core of the program is its implementation of Dijkstra's algorithm to find the shortest path from each `Start` node to its corresponding `End` node.

### Dijkstra's Algorithm (Main Algorithm)

The algorithm explores the grid (a graph) to find the path with the minimum cost (distance). In this implementation, the graph vertices $V$ are the grid cells, and the edges $E$ connect a cell to its 4 adjacent neighbors. Each edge has a uniform weight of 1.

The implementation uses a `std::priority_queue` (binary min-heap) to efficiently select the next node to visit.

#### Time Complexity

* **Worst Case**: $O(V \log V + E)$
* In this grid, the number of vertices $V$ is $N = \text{Rows} \times \text{Cols}$.
* Each vertex has at most 4 edges, so the number of edges $E$ is proportional to $V$ (i.e., $E \approx 4V$).
* Therefore, the complexity simplifies to $O(V \log V + V)$, which is $O(N \log N)$, where $N$ is the total number of cells in the grid.

#### Space Complexity

* **Worst Case**: $O(N)$
* The algorithm maintains data structures to store distances and predecessors for every cell.
    * `dist` vector: $O(V) = O(N)$
    * `parent` vector: $O(V) = O(N)$
* The priority queue (`pq`) can, in the worst case, hold all vertices: $O(V) = O(N)$.
* The total space complexity is therefore dominated by these structures, resulting in $O(N)$.