#include "Actions/Invoker.hpp"
#include <memory>

void Invoker::addCommand(Command *command) { m_queue.emplace(command); }
void Invoker::execute() {
  while (!this->m_queue.empty()) {
    if (this->m_queue.front()->execute())
      this->m_restoreStack.emplace(this->m_queue.front().release());
    this->m_queue.pop();
  }
}

void Invoker::restore(size_t amount) {
  for (size_t i = 0; i < amount && !m_restoreStack.empty(); i++) {
    m_restoreStack.top()->restore();
    m_restoreStack.pop();
  }
}

Invoker &Invoker::get() {
  static Invoker *m_instance = new Invoker();

  return *m_instance;
}
