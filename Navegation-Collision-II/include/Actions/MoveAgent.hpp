#ifndef MOVE_AGENT_HPP
#define MOVE_AGENT_HPP

#include "Actions/Command.hpp"
#include "Math/Vector.hpp"

#include <memory>

namespace Simulation {
class Agent;
}

namespace Commands {
class MoveAgent : public Command {
public:
  MoveAgent(Simulation::Agent *agent, Vec2u last, Vec2u now);
  ~MoveAgent() = default;

  bool execute() override;
  void restore() override;

private:
  Simulation::Agent *m_agent;
  Vec2u m_last;
  Vec2u m_now;
};
} // namespace Commands

#endif
