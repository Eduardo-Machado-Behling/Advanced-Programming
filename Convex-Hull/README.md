# QuickHull for Convex Hulls

This project generates the 2D convex hull for a given set of points using the **QuickHull algorithm**. The implementation follows a divide-and-conquer approach, analogous to the QuickSort algorithm, to recursively partition points and identify the boundary of the smallest convex polygon enclosing all points. The program also benchmarks the execution time against the number of input points and includes a graphical analysis of this relationship.

## 📚 Use Cases

The convex hull is one of the most fundamental structures in computational geometry, serving as a robust approximation of a shape's boundary. Its applications are widespread and practical.

  * **Collision Detection**: In video games and physics simulations, complex object meshes are often simplified to their convex hulls to perform fast and efficient collision checks.
  * **Image Processing**: Used for shape analysis, object detection, and feature extraction. For instance, finding the convex hull of a detected object can help in its identification and orientation analysis.
  * **Geographic Information Systems (GIS)**: Determining the territorial boundary for a cluster of geographic sites or calculating a minimum-area bounding region.
  * **Pattern Recognition**: In statistical data analysis, it can be used to identify the extent of a data cluster and detect outliers.

More info on [Applications of Convex Hull](https://brilliant.org/wiki/convex-hull/)

## 📈 Complexity

The QuickHull algorithm's performance is highly dependent on the distribution of the input points, as this affects the balance of the recursive partitions.

### Time Complexity

  * **Average Case**: $O(N \log N)$
  * **Worst Case**: $O(N^2)$

The worst-case scenario occurs when partitions are consistently unbalanced, for example, when all points lie on or near the final convex hull. The average case, however, is highly efficient for uniformly distributed points.

### Space Complexity

  * **Average Case**: $O(\log N)$
  * **Worst Case**: $O(N)$

The space complexity is determined by the maximum depth of the recursion stack. For balanced partitions, the depth is logarithmic, while for highly skewed partitions, it can become linear.
