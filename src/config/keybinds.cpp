#include "keybinds.h"

#include <cocaine.h>

#include <thread>

// because folding namespaces without indentation is too advanced for vscode
#pragma region fuck
namespace keybinds {
#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39
#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A
#define COMPARISON(str, val) \
  if (vk == str) return val

  uint32_t strToVK(std::string vk) {
    COMPARISON("A", VK_A);
    COMPARISON("B", VK_B);
    COMPARISON("C", VK_C);
    COMPARISON("D", VK_D);
    COMPARISON("E", VK_E);
    COMPARISON("F", VK_F);
    COMPARISON("G", VK_G);
    COMPARISON("H", VK_H);
    COMPARISON("I", VK_I);
    COMPARISON("J", VK_J);
    COMPARISON("K", VK_K);
    COMPARISON("L", VK_L);
    COMPARISON("M", VK_M);
    COMPARISON("N", VK_N);
    COMPARISON("O", VK_O);
    COMPARISON("P", VK_P);
    COMPARISON("Q", VK_Q);
    COMPARISON("R", VK_R);
    COMPARISON("S", VK_S);
    COMPARISON("T", VK_T);
    COMPARISON("U", VK_U);
    COMPARISON("V", VK_V);
    COMPARISON("W", VK_W);
    COMPARISON("X", VK_X);
    COMPARISON("Y", VK_Y);
    COMPARISON("Z", VK_Z);
    COMPARISON("0", VK_0);
    COMPARISON("1", VK_1);
    COMPARISON("2", VK_2);
    COMPARISON("3", VK_3);
    COMPARISON("4", VK_4);
    COMPARISON("5", VK_5);
    COMPARISON("6", VK_6);
    COMPARISON("7", VK_7);
    COMPARISON("8", VK_8);
    COMPARISON("9", VK_9);
    COMPARISON("0", VK_0);
    COMPARISON("_", VK_OEM_MINUS);

    COMPARISON("INS", VK_INSERT);
    COMPARISON("DEL", VK_DELETE);
    COMPARISON("HOM", VK_HOME);
    COMPARISON("PGU", VK_PRIOR);
    COMPARISON("PGD", VK_NEXT);
    COMPARISON("PAU", VK_PAUSE);
    COMPARISON("SCR", VK_SCROLL);
    COMPARISON("+", VK_OEM_PLUS);
    COMPARISON("BCK", VK_BACK);
    COMPARISON("\\|", VK_OEM_5);
    COMPARISON("[{", VK_OEM_4);
    COMPARISON("]}", VK_OEM_6);
    COMPARISON("'\"", VK_OEM_7);
    COMPARISON(";:", VK_OEM_1);
    COMPARISON("/?", VK_OEM_2);
    COMPARISON("`~", VK_OEM_3);
    COMPARISON("ENT", VK_RETURN);
    COMPARISON("SPC", VK_SPACE);
    COMPARISON("CAP", VK_CAPITAL);
    COMPARISON("CTR", VK_CONTROL);
    COMPARISON("ALT", VK_MENU);
    COMPARISON("SHF", VK_SHIFT);
    COMPARISON("TAB", VK_TAB);

    COMPARISON("M1", VK_LBUTTON);
    COMPARISON("M2", VK_RBUTTON);
    COMPARISON("M3", VK_MBUTTON);
    COMPARISON("M4", VK_XBUTTON1);
    COMPARISON("M5", VK_XBUTTON2);
    return 0x0;
    // COMPARISON("=", VK_OEM_);
    // COMPARISON("`", VK_`);
  }

  void handleKey(bool enabled, Keybind key, std::atomic_bool* var) {
    if (enabled) {
      auto vk = strToVK(key.key);
      switch (key.type) {
        case KeybindType::AlwaysOn:
          *var = true;
          break;
        case KeybindType::OnHotkey:
          *var = (GetAsyncKeyState(vk) & 0x8000) != 0;
          break;
        case KeybindType::OffHotkey:
          *var = (GetAsyncKeyState(vk) & 0x8000) == 0;
          break;
        case KeybindType::Toggle:
          if (GetAsyncKeyState(vk) & 1) *var = !*var;
          break;
      }
    } else {
      *var = false;
    }
  }
}  // namespace keybinds
#pragma endregion

namespace keybinds {

  std::atomic_bool esp;
  std::atomic_bool triggerbot;
  void keybindThread() {
    using namespace config;
    while (true) {
      handleKey(true, c.espActivationType, &esp);
      handleKey(c.tbotEnabled, c.tbotKey, &triggerbot);

      // printf("Bitch: %i, %i\n", c.tbotEnabled, triggerbot);
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }
}  // namespace keybinds