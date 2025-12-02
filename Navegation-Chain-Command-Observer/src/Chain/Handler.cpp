#include "Chain/Handler.hpp"

Handler *Handler::setNext(Handler *handler) {
  this->m_next.reset(handler);
  return handler;
}

bool Handler::handle(GridManager::AllocationParam &param) {
  if (!process(param))
    return false;
  if (m_next)
    m_next->handle(param);
  return true;
}
