#ifndef HANDLER_GRID_CONFIG_HPP
#define HANDLER_GRID_CONFIG_HPP

#include "Chain/GridInit/Chain.hpp"

namespace Chain {
class GridConfig : public GridAllocatorChain {
public:
  GridConfig(bool skip = false);
  ~GridConfig() = default;

protected:
  bool process(::GridManager::AllocationParam& param) override;

private:
  bool m_ask;
};
} // namespace Chain

#endif
