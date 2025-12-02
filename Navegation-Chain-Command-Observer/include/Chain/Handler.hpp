#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <memory>

#include "Grid/Manager.hpp"

class Handler {
public:
    virtual ~Handler() {}

    Handler* setNext(Handler* n);

    bool handle(GridManager::AllocationParam& param);

protected:
	std::unique_ptr<Handler> m_next = nullptr;

    virtual bool process(GridManager::AllocationParam& param) = 0; 
};

#endif
