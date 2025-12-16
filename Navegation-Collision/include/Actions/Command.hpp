#ifndef COMMAND_HPP
#define COMMAND_HPP

struct Command {
	virtual ~Command() {}
	virtual void execute() = 0;
};

#endif
