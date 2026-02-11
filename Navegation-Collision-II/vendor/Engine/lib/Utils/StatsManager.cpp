
#include "Utils/StatsManager.hpp"

#include <filesystem>

#ifndef RUNTIME_DIR
#define RUNTIME_DIR "."
#endif

StatsManager &StatsManager::get() {
  static StatsManager *m_instance = new StatsManager();
  return *m_instance;
}

void StatsManager::set(std::string name, StatType value) {
  if (m_vars.contains(name)) {
    m_vars[name] = value;
  } else if (!m_init) {
    m_vars.insert({name, value});
  }
}

std::optional<StatsManager::StatType> StatsManager::get(std::string name) {
  if (m_init && !m_vars.contains(name))
    return std::nullopt;
  return m_vars[name];
}

void StatsManager::log() {
  if (!m_init) {
    m_init = true;

    bool first = true;
    for (auto &[header, _] : m_vars) {
      if (!first) {
        m_file << ',';
      }
      m_file << header;
      first = false;
    }
    m_file << '\n';
  }

  bool first = true;
  for (auto &[_, val] : m_vars) {
    if (!first) {
      m_file << ',';
    }
    std::visit(
        [this](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, std::string>) {
            m_file << '"' << arg << '"';
          } else {
            m_file << arg;
          }
        },
        val);
    first = false;
  }
  m_file << '\n';
}

StatsManager::StatsManager()
    : m_file(std::filesystem::path(RUNTIME_DIR) / "log.csv"), m_init(false) {}
