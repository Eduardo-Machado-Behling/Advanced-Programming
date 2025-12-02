#include "Actions/Invoker.hpp"
#include <memory>

void Invoker::addCommand(Command *command) { m_queue.emplace(command); }
void Invoker::execute() {
  while (!this->m_queue.empty()) {
    this->m_queue.front()->execute();
    this->m_queue.pop();
  }
}

Invoker &Invoker::get() {
  if (!m_instance) {
    m_instance = std::make_unique<Invoker>();
  }

  return *m_instance;
}

std::unique_ptr<Invoker> Invoker::m_instance = nullptr;
