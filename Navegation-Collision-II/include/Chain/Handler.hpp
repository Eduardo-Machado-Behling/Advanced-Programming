#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <memory>

template <class T> class Handler {
public:
  virtual ~Handler() {}

  Handler *setNext(Handler<T> *handler) {
    this->m_next.reset(handler);
    return handler;
  }

  bool handle(T &param) {
    if (!process(param))
      return false;
    if (m_next)
      m_next->handle(param);
    return true;
  }

protected:
  std::unique_ptr<Handler<T>> m_next = nullptr;

  virtual bool process(T &param) = 0;
};

#endif
