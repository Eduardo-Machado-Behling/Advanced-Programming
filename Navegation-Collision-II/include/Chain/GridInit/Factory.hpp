#ifndef HANDLER_GRID_FACTORY_HPP
#define HANDLER_GRID_FACTORY_HPP

#include "Chain/GridInit/Chain.hpp"

namespace Chain {
class GridFactory : public GridAllocatorChain {
public:
  GridFactory();
  ~GridFactory() = default;

  static std::function<GridFactories::IFactory *()> FACTORIES[];

protected:
  bool process(GridManager::AllocationParam &param) override;
};
} // namespace Chain

#endif
