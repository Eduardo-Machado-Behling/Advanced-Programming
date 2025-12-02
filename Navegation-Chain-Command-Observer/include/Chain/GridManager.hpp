#ifndef HANDLER_GRID_MANAGER_HPP
#define HANDLER_GRID_MANAGER_HPP

#include "Chain/Handler.hpp"
#include "Grid/Manager.hpp"

namespace Chain {
class GridManager : public Handler {
public:
  GridManager() = default;
  ~GridManager() = default;

protected:
  bool process(::GridManager::AllocationParam& param) override;
};
} // namespace Chain

#endif
