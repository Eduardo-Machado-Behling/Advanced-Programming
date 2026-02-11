#ifndef PUBLISHER_HPP
#define PUBLISHER_HPP

#include "Observer/Observer.hpp"

#include <memory>
#include <vector>

class Publisher {
public:
  virtual ~Publisher() {}

  void subscribe(Subscriber *obs);
  void unsubscribe(Subscriber *obs);
  void notifySubscribers();

protected:
  std::vector<std::unique_ptr<Subscriber>> m_observers;
};

#endif
