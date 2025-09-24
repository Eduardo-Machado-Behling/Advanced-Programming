#ifndef OBJECTUUID_HPP
#define OBJECTUUID_HPP

#include <cstdint>
#include <iostream>
#include <queue>
#include <utility>
namespace Engine {
namespace Objects {
struct ObjectUUID {
  using UUID = uint32_t;

  ObjectUUID() { available.push(1); }

  UUID get() {
    uint32_t uuid = available.front();
    if (available.size() == 1) {
      available.pop();
      available.push(uuid + 1);
    }

    std::cout << "\nGenerated UUID: " << uuid << " Next = " << available.front()
              << "\n\n";
    return uuid;
  }

  void remove(UUID uuid) { available.push(uuid); }

private:
  std::queue<uint32_t> available;
};
} // namespace Objects
} // namespace Engine

#endif
