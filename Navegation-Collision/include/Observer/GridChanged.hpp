#ifndef SUBSCRIBER_GRID_CHANGED_HPP
#define SUBSCRIBER_GRID_CHANGED_HPP

#include "Observer/Subscriber.hpp"

#include <functional>

namespace Subscribers {
class CallbackSubscriber : public Subscriber {
public:
  using Callback = std::function<void()>;

  ~CallbackSubscriber() = default;

  void setOnChange(Callback callback);
  void update() override;

private:
  Callback m_onChanged;
};
} // namespace Subscribers

#endif
