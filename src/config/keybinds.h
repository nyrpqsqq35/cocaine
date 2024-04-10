#ifndef CONFIG_KEYBIND_H
#define CONFIG_KEYBIND_H

#include <atomic>

namespace keybinds {
  extern std::atomic_bool esp;
  extern std::atomic_bool triggerbot;

  void keybindThread();
}  // namespace keybinds

#endif /* CONFIG_KEYBIND_H */
