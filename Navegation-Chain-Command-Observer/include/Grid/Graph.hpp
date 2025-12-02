#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Math/Vector.hpp"

#include <vector>

namespace Grid {
class IGraph {
public:
  virtual ~IGraph() = default;
  virtual std::vector<Vec2u> getNeighbors(Vec2u coord) = 0;
  virtual bool isValid(Vec2u coord) = 0;
};
} // namespace Grid

#endif // GRAPH_HPP