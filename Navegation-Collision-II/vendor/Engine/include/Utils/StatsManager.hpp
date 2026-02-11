
#ifndef UTILS_STATSMANAGER_HPP
#define UTILS_STATSMANAGER_HPP

#include <cstdint>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

#include "engine_api.hpp"

class ENGINE_API StatsManager {
public:
  using StatType = std::variant<uint64_t, int64_t, double, std::string, bool>;

  static StatsManager &get();

  void set(std::string name, StatType value);
  std::optional<StatsManager::StatType> get(std::string name);
  void log();

private:
  StatsManager();

  std::ofstream m_file;
  std::unordered_map<std::string, StatType> m_vars;
  bool m_init;
};

#endif // UTILS_STATSMANAGER_HPP