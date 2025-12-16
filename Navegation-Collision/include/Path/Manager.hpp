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

  struct BroadCollisionData {
    struct {
      double entry;
      double exit;
    } t;
    PathID *id;

    BroadCollisionData(double entry, double exit, PathID *id) {
      t.entry = entry;
      t.exit = exit;

      id = id;
    }
  };

  // struct BroadCollision {
  //   PathID *agents[2];
  //
  //   BroadCollision(PathID *a, PathID *b) : agents{a, b} {}
  // };

public:
  struct PathCollisionData {
    double t;
    PathID *agents[2];
	Vec2u gridPos;

    PathCollisionData(double t, PathID *a, PathID *b, Vec2u grid) : t(t), agents{a, b}, gridPos(grid) {}
  };

  struct DynamicInfo {
    double velocity = 1.0;
    Vec2 size;
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

  EvalInfo evalPosition(double t, PathID *id);

private:
  PathManager();

  static std::unique_ptr<PathManager> m_instance;

  double longestPath;
  Vec2u mapConfig;
  std::vector<std::unordered_map<double, PathID *>> m_pathMap;
  std::list<std::vector<Vec2u>> m_paths;
  std::vector<PathCollisionData> m_broadColl;
  std::vector<std::vector<BroadCollisionData>> m_broadCandidatesColl;

  friend std::unordered_map<PathID, std::vector<PathID>, PathIDHasher>;
  std::unordered_map<PathID *, std::pair<size_t, std::vector<PathID *>>,
						 PathIDHasher> m_collObjs;

  Subscribers::CallbackSubscriber m_onGridLayoutChange;
};

#endif
