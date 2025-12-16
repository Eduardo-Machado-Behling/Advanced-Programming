#ifndef INVOKER_HPP
#define INVOKER_HPP

#include <memory>
#include <queue>

#include "Actions/Command.hpp"

class Invoker{
	public:
		void addCommand(Command* command);
		void execute();

		static Invoker& get();

		void redo(size_t i);

	private:
		std::queue<std::unique_ptr<Command>> m_queue;
		static std::unique_ptr<Invoker> m_instance;
};

#endif
