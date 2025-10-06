# Voronoi Diagrams and Delaunay Triangulations
This project generates 2D Voronoi diagrams and their corresponding Delaunay triangulations using a brute-force implementation. The method is intuitive, mimicking how one might construct the diagram on paper by relying on the fundamental geometric principles of line-segment intersections to define each cell. The program benchmarks the execution time for both calculations against the number of input points and includes a graphical analysis of this relationship.

## Use Cases 
Voronoi diagrams are a fundamental concept in computational geometry with numerous real-world applications. They are used to model and analyze spatial data where proximity is key.

- Natural Structures: Simulating cellular patterns, crystal grains, or forest canopies.

- Logistics and Services: Determining the closest hospital, school, or service center from any location, or defining the coverage area for cell towers.

- Ecology: Modeling the territories of animals or the competitive regions of plants.

More info on [The Fascinating World of Voronoi Diagrams](https://medium.com/data-science/the-fascinating-world-of-voronoi-diagrams-da8fc700fa1b)

## Complexity
Brute-Force Intersection of Half-Planes as in [book](https://www.amazon.com/Computational-Geometry-Applications-Mark-Berg/dp/3642096816).

Time Complexity: O(N³)

Where N is the number of input points (sites).

Space Complexity: O(N)

The storage required for the input sites, the vertices and edges of the Voronoi cells, and the final Delaunay edges all scales linearly with the number of input points P.

