#ifndef HANDLER_GRID_FACTORY_HPP
#define HANDLER_GRID_FACTORY_HPP

#include "Chain/GridInit/Chain.hpp"

namespace Chain {
class GridFactory : public GridAllocatorChain {
public:
  GridFactory(bool ask = true);
  ~GridFactory() = default;

protected:
  bool process(GridManager::AllocationParam &param) override;
private:
  bool m_ask;
};
} // namespace Chain

#endif
