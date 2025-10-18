# Simple 2D Rendering Engine (OpenGL)

Simple 2D Rendering for Points, Lines and Polygons for algorithm visualization

## Dependecies
### C++
- GLAD
- GLFW
- OpenGL >=4.3
### Python
- matplotlib
- seaborn
- pandas

## Building
### Engine
To build use cmake, the dependencies should be fetched and built alongside
```sh
# the building of the Demo can be disable with -DBUILD_DEMO=OFF
$ cmake -S . -B build

$ cmake --build build
$ ./bin/demo
```
### Graphing
To generate graphs use Python powered by the dependencies cited above
```sh
$ ./bin/demo

# if you don't have the dependencies
$ pip install -r requirements.txt

# this generate a "Metrics.png" with the graphs from "log.csv" data
$ python graph.py
```

