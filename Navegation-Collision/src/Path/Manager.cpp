#include "Path/Manager.hpp"

#include "Grid/Manager.hpp"
#include "Path/Dijkstra.hpp"

#include <cstddef>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

struct PathManager::PathID {
  using Iterator = std::list<std::vector<Vec2u>>::iterator;
  Iterator list;
  DynamicInfo info;

  PathID(Iterator l, DynamicInfo i) : list(l), info(i) {}

  bool operator==(const PathID &other) const { return list == other.list; }
};

size_t PathManager::PathIDHasher::operator()(const PathID *k) const {
  return std::hash<const std::vector<Vec2u> *>{}(&*(*k).list);
}

PathManager &PathManager::get() {
  if (!m_instance) {
    m_instance.reset(new PathManager());
  }

  return *m_instance;
}

void PathManager::PathDeleter::operator()(PathID *p) const { delete p; }

PathManager::PathPtr PathManager::getId(DynamicInfo info) {
  m_paths.emplace_front();
  return PathPtr(new PathID(m_paths.begin(), info));
}

PathManager &PathManager::getPath(PathManager::PathID *id, Vec2u start,
                                  Vec2u end, Grid::IGraph &graph) {
  std::vector<Vec2u> path = Dijkstra::ShortestPath(start, end, graph);

  double step = 1 / id->info.velocity;

  for (size_t i = 0; i < path.size(); i++) {
    Vec2u curr = path[i];
    double t = i / id->info.velocity;

    double tEntry = i * step;
    double tExit = (i + 1) * step;

    size_t mapIdx = curr[1] * mapConfig[0] + curr[0];

    for (const auto &entry : m_broadCandidatesColl[mapIdx]) {
      if (tEntry < entry.t.exit && tExit > entry.t.entry) {
        m_broadColl.emplace_back(tEntry, id, entry.id);
      }
    }

    m_broadCandidatesColl[mapIdx].emplace_back(tEntry, tExit, id);
  }

  *id->list = std::move(path);
  return *this;
}

static bool collides(Vec2 a, Vec2 aDim, Vec2 b, Vec2 bDim) {
  Vec2 aHalf = aDim * 0.5f;
  Vec2 bHalf = bDim * 0.5f;

  Vec2 diff = a - b;

  bool overlapX = std::abs(diff[0]) <= (aHalf[0] + bHalf[0]);
  bool overlapY = std::abs(diff[1]) <= (aHalf[1] + bHalf[1]);

  return overlapX && overlapY;
}

std::vector<PathManager::PathCollisionData> PathManager::getCollisions() {
  std::vector<PathCollisionData> realCollisions;

  for (const auto &broad : m_broadColl) {
    double t = broad.t;

	auto resA = evalPosition(t, broad.agents[0]);
    Vec2 posA = resA.position;
    Vec2 posB = evalPosition(t, broad.agents[1]).position;

    if (collides(posA, broad.agents[0]->info.size, posB,
                 broad.agents[1]->info.size)) {
      realCollisions.emplace_back(t, broad.agents[0], broad.agents[1], resA.grid);
    }
  }

  m_broadColl.clear();

  return realCollisions;
}
PathManager::EvalInfo PathManager::evalPosition(double t, PathID *id) {
  if (!id) {
    throw std::invalid_argument("id is null");
  }
  double step = 1 / id->info.velocity;

  size_t i = t / step;
  size_t ni = i + 1;

  std::cout << "t = " << t << ", step=" << step << ", i= " << i
            << ", ni =" << ni << ", max =" << id->list->size() << '\n';

  GridManager &grid = GridManager::get();
  if (ni >= id->list->size()) {
    std::cout << "end\n";
    return {.position = grid.getCenter(id->list->back()),
            .grid = id->list->back(),
            .i = i,
            .ended = true};
  }

  Vec2u gridPos = (*id->list)[i];
  Vec2 iPos = grid.getCenter((*id->list)[i]);
  Vec2 niPos = grid.getCenter((*id->list)[ni]);

  double lerpT = (t - step * i) / step;
  Vec2 lerp = iPos + (niPos - iPos) * lerpT;

  return {.position = lerp, .grid = gridPos, .i = i, .ended = false};
}

PathManager::PathManager() {
  auto update = [this]() {
    mapConfig[0] = GridManager::get().rows();
    mapConfig[1] = GridManager::get().cols();

    size_t gridSize = mapConfig[0] * mapConfig[1];
    std::cout << "[PATH]: update: " << gridSize << '\n';
    m_pathMap.resize(gridSize);
    m_broadCandidatesColl.resize(gridSize);
  };

  update();
  m_onGridLayoutChange.setOnChange(update);

  GridManager::get().subscribeOnGridChange(&m_onGridLayoutChange);
}

std::unique_ptr<PathManager> PathManager::m_instance = nullptr;
