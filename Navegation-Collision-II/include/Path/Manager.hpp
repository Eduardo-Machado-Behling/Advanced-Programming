#ifndef PATH_MANAGER_HPP
#define PATH_MANAGER_HPP

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

#include "Grid/Graph.hpp"
#include "Math/Vector.hpp"
#include "Observer/GridChanged.hpp"

class PathManager {
public:
  // Forward declare the internal ID structure
  struct PathID;

  // Custom deleter for the smart pointer
  struct PathDeleter {
    void operator()(PathID *p) const;
  };

  // The Handle: Agents own this. When it dies, the path is removed.
  using PathPtr = std::unique_ptr<PathID, PathDeleter>;
  using OnChangeCallback = std::function<void()>;

  struct DynamicInfo {
    double velocity = 1.0;
    double radius = 10.0;
  };

  static PathManager &get();

  // 1. Create a path and get ownership
  PathPtr requestPath(DynamicInfo info, Vec2u start, Vec2u end);

  void registerOnChange(PathID *id, OnChangeCallback callback);

  // 2. Get the next waypoint (Thread-safe read)
  std::optional<Vec2> getSegment(PathID *id, size_t i);

  // 3. Recalculate all empty/dirty paths (Call this in your Game Loop)
  void update();

  // 4. Force all paths to recalculate (e.g., on Grid Change)
  void invalidateAll();

private:
  PathManager();
  ~PathManager();

  // Internal methods used by PathDeleter
  void unregisterPath(PathID *id);

  // --- Data Storage ---

  // The actual path data. Stable iterators are crucial here.
  std::list<std::vector<Vec2u>> m_pathStore;

  // The Registry: A set of raw pointers to currently active IDs.
  // We do NOT own them here; the Agent (PathPtr) owns them.
  std::unordered_set<PathID *> m_registry;

  // Observer for Grid changes
  Subscribers::CallbackSubscriber m_onGridChange;
};

#endif // PATH_MANAGER_HPP