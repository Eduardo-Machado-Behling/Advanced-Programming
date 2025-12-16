#include "Path/Manager.hpp"

#include "Grid/Manager.hpp"
#include "Path/Dijkstra.hpp"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
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

void PathManager::PathDeleter::operator()(PathID *p) const {
  if (m_instance)
    m_instance->removePath(p);

  delete p;
}

PathManager::PathPtr PathManager::getId(DynamicInfo info) {
  m_paths.emplace_front();
  return PathPtr(new PathID(m_paths.begin(), info));
}

PathManager &PathManager::getPath(PathManager::PathID *id, Vec2u start,
                                  Vec2u end, Grid::IGraph &graph) {
  if (!id->list->empty()) {
    return *this;
  }

  std::vector<Vec2u> path = Dijkstra::ShortestPath(start, end, graph);

  double step = 1 / id->info.velocity;

  for (size_t i = 0; i < path.size(); i++) {
    Vec2u curr = path[i];
    double t = i / id->info.velocity;

    TimeFrame tFrame{.start = i * step, .end = (i + 1) * step};

    size_t mapIdx = curr[1] * m_mapConfig[0] + curr[0];

    auto &candidates = m_broadCandidatesColl[mapIdx];
    size_t count = candidates.size();

    for (size_t k = 0; k < count; ++k) {
      const auto &entry = candidates[k];

      if (entry.id == id)
        continue;

      if (tFrame.start < entry.t.end && tFrame.end > entry.t.start) {
        double overlapStart = std::max(tFrame.start, entry.t.start);
        double overlapEnd = std::min(tFrame.end, entry.t.end);

        m_broadColl.emplace_back(tFrame, id, entry.id);
      }
    }

    m_broadCandidatesColl[mapIdx].emplace_back(tFrame, id);
  }

  *id->list = std::move(path);
  return *this;
}

struct SweptCircleResult {
  bool collides;
  double normalizedT;

  SweptCircleResult(bool collides, double normalizedT = 0.0)
      : collides(collides), normalizedT(normalizedT) {}
};

struct SweptCircleComponent {
  Vec2 start;
  Vec2 end;
  double radius;
};

static SweptCircleResult sweptCircle(SweptCircleComponent A,
                                     SweptCircleComponent B) {
  Vec2 vA = A.end - A.start;
  Vec2 vB = B.end - B.start;
  Vec2 vRel = vA - vB;
  Vec2 pRel = A.start - B.start;

  double radiusSum = A.radius + B.radius;
  double radiusSumSq = radiusSum * radiusSum;

  struct {
    double A;
    double B;
    double C;

    inline double discriminant() {
      if (isnan(m_discriminant))
        m_calcDiscriminant();

      return m_discriminant;
    }

    inline double x() {
      if (isnan(m_x))
        m_calcSolve();

      return m_x;
    }

  private:
    inline void m_calcDiscriminant() { m_discriminant = B * B - 4 * A * C; }
    inline void m_calcSolve() {
      m_x = (-B - std::sqrt(m_discriminant)) / (2.0 * A);
    }
    double m_discriminant = NAN;
    double m_x = NAN;

  } quadratic;

  quadratic.A = vRel.dot(vRel);

  quadratic.B = 2.0 * pRel.dot(vRel);

  quadratic.C = pRel.dot(pRel) - radiusSumSq;

  if (quadratic.C <= 0) {
    return {true};
  }

  if (std::abs(quadratic.A) < 1e-6) {
    return {false};
  }

  if (quadratic.discriminant() < 0.0) {
    return {false};
  }

  double t = quadratic.x();

  if (t >= 0.0 && t <= 1.0) {
    return {true, t};
  }

  return {false};
}

std::vector<PathManager::PathCollisionData> PathManager::getCollisions() {
  std::vector<PathCollisionData> realCollisions;

  for (const auto &broad : m_broadColl) {
    double tStart = broad.t.start;
    double tEnd = broad.t.end;

    auto startResA = evalPosition(tStart, broad.agents[0]);
    auto startResB = evalPosition(tStart, broad.agents[1]);

    auto endResA = evalPosition(tEnd, broad.agents[0]);
    auto endResB = evalPosition(tEnd, broad.agents[1]);

    if (startResA.ended || startResB.ended)
      continue;

    double radiusA = broad.agents[0]->info.radius;
    double radiusB = broad.agents[1]->info.radius;

    auto result = sweptCircle({startResA.position, endResA.position, radiusA},
                              {startResB.position, endResB.position, radiusB});

    if (result.collides) {
      double normalizedTime = result.normalizedT;
      double collisionTime = tStart + (tEnd - tStart) * normalizedTime;
      Vec2 posA = startResA.position +
                  (endResA.position - startResA.position) * normalizedTime;
      Vec2 posB = startResB.position +
                  (endResB.position - startResB.position) * normalizedTime;

      realCollisions.push_back(
          {collisionTime, broad.agents[0], broad.agents[1], posA, posB});
    }
  }

  return realCollisions;
}
PathManager::EvalInfo PathManager::evalPosition(double t, PathID *id) {
  if (!id) {
    throw std::invalid_argument("id is null");
  }
  double step = 1 / id->info.velocity;

  size_t i = t / step;
  size_t ni = i + 1;

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

void PathManager::removePath(PathID *id) {
  if (!m_alive || !id)
    return;

  const std::vector<Vec2u> &pathPoints = *id->list;
  double step = 1.0 / id->info.velocity;

  for (size_t i = 0; i < pathPoints.size(); ++i) {
    Vec2u curr = pathPoints[i];

    size_t mapIdx = curr[1] * m_mapConfig[0] + curr[0];

    if (mapIdx < m_broadCandidatesColl.size()) {
      auto &cell = m_broadCandidatesColl[mapIdx];

      cell.erase(std::remove_if(cell.begin(), cell.end(),
                                [id](const BroadCollisionData &entry) {
                                  return entry.id == id;
                                }),
                 cell.end());
    }
  }

  m_broadColl.erase(std::remove_if(m_broadColl.begin(), m_broadColl.end(),
                                   [id](const BroadCollisionEntry &entry) {
                                     return entry.agents[0] == id ||
                                            entry.agents[1] == id;
                                   }),
                    m_broadColl.end());

  m_paths.erase(id->list);
}

PathManager::PathManager() {
  auto update = [this]() {
    m_mapConfig[0] = GridManager::get().rows();
    m_mapConfig[1] = GridManager::get().cols();

    size_t gridSize = m_mapConfig[0] * m_mapConfig[1];
    std::cout << "[PATH]: update: " << gridSize << '\n';
    m_pathMap.resize(gridSize);
    m_broadCandidatesColl.resize(gridSize);
    m_alive = true;
  };

  update();
  m_onGridLayoutChange.setOnChange(update);

  GridManager::get().subscribeOnGridChange(&m_onGridLayoutChange);
}

PathManager::~PathManager() { m_alive = false; }

std::unique_ptr<PathManager> PathManager::m_instance = nullptr;

std::ostream &operator<<(std::ostream &os,
                         const PathManager::PathCollisionData &data) {
  os << "[Collision t=" << std::fixed << std::setprecision(3) << data.t << "] "
     << "Agent A(" << data.agents[0].agent << ") @ "
     << "(" << data.agents[0].pos[0] << ", " << data.agents[0].pos[1] << ") "
     << "<--> "
     << "Agent B(" << data.agents[1].agent << ") @ "
     << "(" << data.agents[1].pos[0] << ", " << data.agents[1].pos[1] << ")";
  return os;
}

std::ostream &
operator<<(std::ostream &os,
           const std::vector<PathManager::PathCollisionData> &collisions) {
  os << "--- Collision Report (" << collisions.size() << " events) ---\n";
  if (collisions.empty()) {
    os << "(No Collisions)\n";
  } else {
    for (size_t i = 0; i < collisions.size(); ++i) {
      os << "#" << i << ": " << collisions[i] << "\n";
    }
  }
  os << "-----------------------------------";
  return os;
}