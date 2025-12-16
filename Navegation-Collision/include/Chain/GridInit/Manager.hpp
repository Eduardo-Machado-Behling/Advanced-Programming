#ifndef HANDLER_GRID_MANAGER_HPP
#define HANDLER_GRID_MANAGER_HPP

#include "Chain/GridInit/Chain.hpp"

namespace Chain {
class GridManager : public GridAllocatorChain {
public:
  GridManager() = default;
  ~GridManager() = default;

protected:
  bool process(::GridManager::AllocationParam& param) override;
};
} // namespace Chain

#endif
