#ifndef OBJECTUUID_HPP
#define OBJECTUUID_HPP

#include <cstdint>
#include <iostream>
#include <queue>
#include <stack>
#include <utility>
namespace Engine {
namespace Objects {
struct ObjectUUID {
  using UUID = uint32_t;

  ObjectUUID() { available.push(1); }

  UUID get() {
    uint32_t uuid = available.top();
    if (available.size() == 1) {
      available.pop();
      available.push(uuid + 1);
    } else {
      available.pop();
    }

    return uuid;
  }

  void reset() {
    while (!available.empty()) {
      available.pop();
    }

    available.push(1);
  }
  void remove(UUID uuid) { available.push(uuid); }

private:
  std::stack<uint32_t> available;
};
} // namespace Objects
} // namespace Engine

#endif
