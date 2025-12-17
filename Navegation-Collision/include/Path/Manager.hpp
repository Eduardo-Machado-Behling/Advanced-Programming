#ifndef PATH_MANAGER_HPP
#define PATH_MANAGER_HPP

#include <cstdlib>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Grid/Graph.hpp"
#include "Math/Vector.hpp"

#include "Observer/GridChanged.hpp"

class PathManager {
public:
  struct PathID;
  struct PathDeleter {
    void operator()(PathID *p) const;
  };
  using PathPtr = std::unique_ptr<PathID, PathDeleter>;

private:
  struct PathIDHasher {
    std::size_t operator()(const PathID *k) const;
  };

  using PathContainer = std::list<std::vector<Vec2>>;

  struct TimeFrame {
    double start;
    double end;
  };

  struct BroadCollisionData {
    TimeFrame t;
    PathID *id;

    BroadCollisionData(TimeFrame t, PathID *id) : t(t), id(id) {}
  };

  struct BroadCollisionEntry {
    TimeFrame t;
    PathID *agents[2];

    BroadCollisionEntry(TimeFrame t, PathID *A, PathID *B) : t(t) {
      agents[0] = A;
      agents[1] = B;
    }
  };

public:
  struct PathCollisionData {
    double t;
    struct {
      PathID *agent;
      Vec2 pos;
    } agents[2];

    PathCollisionData(double t, PathID *a, PathID *b, Vec2 aPos, Vec2 bPos)
        : t(t) {
      agents[0] = {.agent = a, .pos = aPos};
      agents[1] = {.agent = b, .pos = bPos};
    }
  };

  void removePath(PathID *id);

  struct DynamicInfo {
    double velocity = 1.0;
    double radius;
  };

  struct EvalInfo {
    Vec2 position;
    Vec2u grid;
    size_t i;
    bool ended;
  };

  static PathManager &get();

  PathPtr getId(DynamicInfo info);
  PathManager &getPath(PathID *id, Vec2u start, Vec2u end, Grid::IGraph &graph);
  std::vector<PathCollisionData> getCollisions();

  void destroy();

  EvalInfo evalPosition(double t, PathID *id);

  ~PathManager();

private:
  PathManager();

  static std::unique_ptr<PathManager> m_instance;

  bool m_alive;
  double m_longestPath;
  Vec2u m_mapConfig;
  std::vector<std::unordered_map<double, PathID *>> m_pathMap;

  struct Paths {
    std::list<std::vector<Vec2u>> &operator*() { return m_data; }
    std::list<std::vector<Vec2u>> *operator->() { return &m_data; }

    bool alive() const { return m_alive; }
    void die() { m_alive = false; }

    ~Paths() { m_alive = false; }

  private:
    std::list<std::vector<Vec2u>> m_data;
    bool m_alive = true;
  } m_paths;
  std::vector<BroadCollisionEntry> m_broadColl;
  std::vector<std::vector<BroadCollisionData>> m_broadCandidatesColl;

  friend std::unordered_map<PathID, std::vector<PathID>, PathIDHasher>;
  std::unordered_map<PathID *, std::pair<size_t, std::vector<PathID *>>,
                     PathIDHasher>
      m_collObjs;

  Subscribers::CallbackSubscriber m_onGridLayoutChange;
};

std::ostream &operator<<(std::ostream &os,
                         const PathManager::PathCollisionData &data);

std::ostream &
operator<<(std::ostream &os,
           const std::vector<PathManager::PathCollisionData> &collisions);

#endif
