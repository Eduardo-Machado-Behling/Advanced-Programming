#ifndef PUBLISHER_HPP
#define PUBLISHER_HPP

#include "Observer/Subscriber.hpp"

#include <vector>

class Publisher {
public:
  virtual ~Publisher() {}

  void subscribe(Subscriber *obs);
  void unsubscribe(Subscriber *obs);
  void notifySubscribers() const;

protected:
  std::vector<Subscriber *> m_observers;
};

#endif
