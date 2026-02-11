#include "Chain/GridInit/Config.hpp"

namespace Chain {
GridConfig::GridConfig() {}
bool GridConfig::process(GridManager::AllocationParam &param) {
  return param.gridConfig[0] > 0 && param.gridConfig[1] > 0;
}
} // namespace Chain
