# Minkowski Sum Implementation 

This project presents an implementation of the 2D Minkowski Sum algorithm. This technique is applied to generate configuration space obstacles (C-obstacles), a fundamental component in robotic motion planning used to map permissible and non-permissible locations for a robot within an environment.

As a prerequisite processing step, the application first computes the Convex Hull for both the robot and obstacle polygons utilizing the QuickHull algorithm. Following this, the Minkowski Sum is calculated between each obstacle and the reflected robot hull to produce the final set of C-obstacles.


## 📈 Complexity

The overall performance of the program is dictated by its two distinct computational phases.

### QuickHull (Preprocessing)

#### Time Complexity

  * **Average Case**: $O(N \log N)$
  * **Worst Case**: $O(N^2)$

#### Space Complexity:

  * **Average Case**: $O(\log N)$
  * **Worst Case**: $O(N)$

### Minkowski Sum (Main Algorithm)

#### Time Complexity:
  * **Worst Case**: $O(m + n)$, where $m$ and $n$ are the vertex counts of the two convex hulls. This operation is linear with respect to the size of its inputs.

#### Space Complexity:
  * **Worst Case**: $O(1)$.
