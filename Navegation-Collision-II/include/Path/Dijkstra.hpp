#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <vector>

#include "Grid/Graph.hpp"
#include "Math/Vector.hpp"
#include "Path/Manager.hpp"

class Dijkstra {
  struct Node {
    Vec2u pos;
    int dist;

    Node(int dist, Vec2u pos) : dist(dist), pos(pos) {}

    bool operator>(const Node &other) const { return dist > other.dist; }
  };

  static std::vector<Vec2u> ShortestPath(Vec2u start, Vec2u end,
                                         Grid::IGraph *graph);

  friend PathManager;
};

#endif // DIJKSTRA_HPP
