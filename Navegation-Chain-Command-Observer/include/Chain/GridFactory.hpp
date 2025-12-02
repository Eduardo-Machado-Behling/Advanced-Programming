#ifndef HANDLER_GRID_FACTORY_HPP
#define HANDLER_GRID_FACTORY_HPP

#include "Chain/Handler.hpp"

namespace Chain {
class GridFactory : public Handler {
public:
  GridFactory() = default;
  ~GridFactory() = default;

protected:
  bool process(GridManager::AllocationParam &param) override;
};
} // namespace Handlers

#endif
