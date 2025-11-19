#include "Dijkstra.hpp"

#include "Grid/Manager.hpp"

std::vector<Vec2u> Dijkstra::ShortestPath(Vec2u start, Vec2u end) {
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

  int dr[] = {-1, 1, 0, 0}; // Up, Down
  int dc[] = {0, 0, -1, 1}; // Left, Right

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

    for (int i = 0; i < 4; ++i) {
      int nr = (int)u[1] + dr[i];
      int nc = (int)u[0] + dc[i];

      if (nr < 0 || nr >= (int)grid.rows() || nc < 0 || nc >= (int)grid.cols())
        continue;

      auto cell = grid.get(nr, nc);

      bool isStartOrEnd = (nr == (int)start[1] && nc == (int)start[0]) ||
                          (nr == (int)end[1] && nc == (int)end[0]);

      if (cell->empty() || isStartOrEnd) {
        int newDist = dist[u[1]][u[0]] + 1;

        if (newDist < dist[nr][nc]) {
          dist[nr][nc] = newDist;
          parent[nr][nc] = u;
          pq.push({newDist, {(uint32_t)nc, (uint32_t)nr}});
        }
      }
    }
  }

  std::vector<Vec2u> path;

  if (dist[end[1]][end[0]] == std::numeric_limits<int>::max()) {
    return path; // Return empty path
  }

  Vec2u curr = end;
  while (!(curr[1] == UINTMAX && curr[0] == UINTMAX)) {
    path.push_back(curr);
    curr = parent[curr[1]][curr[0]];
  }

  reverse(path.begin(), path.end());

  return path;
}
