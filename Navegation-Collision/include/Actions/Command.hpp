#ifndef COMMAND_HPP
#define COMMAND_HPP

struct Command {
  virtual ~Command() {}
  virtual bool execute() = 0;
  virtual void restore() {}
};

#endif
