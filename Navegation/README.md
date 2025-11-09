# Navegation

This project is an interactive C++/GLFW application that implements a grid-based navigation algorithm. It uses **Dijkstra's algorithm** to find and visualize the shortest path between multiple start and end points on a 2D grid populated with obstacles.

The application allows for interactive creation of the grid, placement of obstacles, and definition of multiple "agent" paths. It also includes a command-line benchmarking mode to generate performance data for analysis, fulfilling the "graphs" requirement of the assignment.

## Interactive Controls

* **Define Grid Area:** `Left-Click` and drag to define the bounding box of the grid. This will create a custom grid within that area (the rows and columns amount will be prompt in the terminal).

* **Place Obstacle:**
    * `Left-Click (Hold)` and drag over grid cells.
    * OR, press `F2` while hovering over a cell.

* **Define Start/End Pair:** `F1 (Hold)` over the start cell, drag to the end cell, and `F1 (Release)`.

* **Generate Random Agents:** Press `F3` to add a random number of Start/End pairs to the current grid (you will be prompted in the console for the amount).

* **Clear Cell:** `Right-Click (Hold)` and drag over an obstacle, start, or end cell to clear it.

* **Run Pathfinding:** Press `SPACE` to run Dijkstra's algorithm for all defined start/end pairs. The paths will be animated.

* **Clear Grid:** Press `Q` to deallocate the grid and start over.

* **Exit:** Press `ESCAPE`.

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