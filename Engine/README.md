# Simple 2D Rendering Engine (OpenGL)

Simple 2D Rendering for Points, Lines and Polygons for algorithm visualization

## Dependecies
- GLAD
- GLFW
- OpenGL >=4.3

## Building
To build use cmake, the dependencies should be fetched and built alongside
```sh
# the building of the Demo can be disable with -DBUILD_DEMO=OFF
$ cmake -S . -B build

$ cmake --build build
$ ./bin/demo
```
