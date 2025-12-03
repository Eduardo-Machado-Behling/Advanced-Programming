#include "Observer/Publisher.hpp"
#include "Observer/Subscriber.hpp"

#include <algorithm>

void Publisher::subscribe(Subscriber *obs) { m_observers.push_back(obs); }

void Publisher::unsubscribe(Subscriber *obs) {
  m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), obs));
}

void Publisher::notifySubscribers() const {
  for (Subscriber *sub : m_observers) {
    sub->update();
  }
}
