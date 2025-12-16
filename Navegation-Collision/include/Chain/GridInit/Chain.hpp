#ifndef GRID_INIT_CHAIN_HPP
#define GRID_INIT_CHAIN_HPP

#include "Chain/Handler.hpp"
#include "Grid/Manager.hpp"

namespace Chain {
class GridAllocatorChain : public Handler<GridManager::AllocationParam> {
public:
  GridAllocatorChain() = default;
  ~GridAllocatorChain() = default;

protected:
  virtual bool process(GridManager::AllocationParam &param) override {
    return true;
  }
};
} // namespace Chain

#endif
