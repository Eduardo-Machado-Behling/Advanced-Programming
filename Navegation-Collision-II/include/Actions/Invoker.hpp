#ifndef INVOKER_HPP
#define INVOKER_HPP

#include <limits>
#include <memory>
#include <queue>
#include <stack>

#include "Actions/Command.hpp"

class Invoker {
public:
  void addCommand(Command *command);
  void execute();

  static Invoker &get();

  void restore(size_t amount = std::numeric_limits<size_t>::max());

private:
  std::queue<std::unique_ptr<Command>> m_queue;
  std::stack<std::unique_ptr<Command>> m_restoreStack;
};

#endif
