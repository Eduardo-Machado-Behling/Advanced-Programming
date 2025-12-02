#include "Chain/GridManager.hpp"

namespace Chain {

bool GridManager::process(::GridManager::AllocationParam &param) {
  ::GridManager::get().allocate(param);
  return true;
}

} // namespace Chain
