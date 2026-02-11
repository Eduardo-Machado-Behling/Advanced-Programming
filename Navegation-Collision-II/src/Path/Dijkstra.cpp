#include "Path/Dijkstra.hpp"

#include "Grid/Manager.hpp"

#include <algorithm>

std::vector<Vec2u> Dijkstra::ShortestPath(Vec2u start, Vec2u end,
                                          Grid::IGraph *graph) {
  GridManager &grid = GridManager::get();
  if (!grid.allocated())
    return {};

  const uint32_t UINTMAX = std::numeric_limits<uint32_t>::max();

  std::vector<std::vector<int>> dist(
      grid.rows(),
      std::vector<int>(grid.cols(), std::numeric_limits<int>::max()));
  std::vector<std::vector<Vec2u>> parent(
      grid.rows(), std::vector<Vec2u>(grid.cols(), {UINTMAX, UINTMAX}));

  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

  dist[start[1]][start[0]] = 0;
  pq.push({0, start});

  while (!pq.empty()) {
    Node current = pq.top();
    pq.pop();

    int d = current.dist;
    Vec2u u = current.pos;

    if (u == end) {
      break;
    }

    if (d > dist[u[1]][u[0]]) {
      continue;
    }

    for (Vec2u next : graph->getNeighbors(u)) {
      auto cell = grid.get(next);

      bool isStartOrEnd = next == start || next == end;

      if (!cell->isBlocking()) {
        int newDist = dist[u[1]][u[0]] + 1;

        if (newDist < dist[next[1]][next[0]]) {
          dist[next[1]][next[0]] = newDist;
          parent[next[1]][next[0]] = u;
          pq.push({newDist, next});
        }
      }
    }
  }

  std::vector<Vec2u> path;

  if (dist[end[1]][end[0]] == std::numeric_limits<int>::max()) {
    return path;
  }

  Vec2u curr = end;
  while (!(curr[1] == UINTMAX && curr[0] == UINTMAX)) {
    path.push_back(curr);
    curr = parent[curr[1]][curr[0]];
  }

  std::reverse(path.begin(), path.end());

  return path;
}
