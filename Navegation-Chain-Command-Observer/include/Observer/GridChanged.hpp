#ifndef SUBSCRIBER_GRID_CHANGED_HPP
#define SUBSCRIBER_GRID_CHANGED_HPP

#include "Observer/Subscriber.hpp"

#include <functional>

namespace Subscribers {
class GridChanged : public Subscriber {
public:
  using Callback = std::function<void()>;

  ~GridChanged() = default;

  void setOnChange(Callback callback);
  void update() override;

private:
  Callback m_onChanged;
};
} // namespace Subscribers

#endif
