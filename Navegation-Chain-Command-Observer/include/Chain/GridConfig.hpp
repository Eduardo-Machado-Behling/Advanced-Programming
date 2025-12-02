#ifndef HANDLER_GRID_CONFIG_HPP
#define HANDLER_GRID_CONFIG_HPP

#include "Chain/Handler.hpp"

namespace Chain {
class GridConfig : public Handler {
public:
  GridConfig() = default;
  ~GridConfig() = default;

protected:
  bool process(::GridManager::AllocationParam& param) override;
};
} // namespace Chain

#endif
