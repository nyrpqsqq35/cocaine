#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <nlohmann/json.hpp>

#include "Color.h"

// https://www.scs.stanford.edu/~dm/blog/va-opt.html
// EXPAND4 interp 256 times
#define NJSON_PARENS ()
#define NJSON_EXPAND(arg) \
  NJSON_EXPAND1(NJSON_EXPAND1(NJSON_EXPAND1(NJSON_EXPAND1(arg))))
#define NJSON_EXPAND1(arg) \
  NJSON_EXPAND2(NJSON_EXPAND2(NJSON_EXPAND2(NJSON_EXPAND2(arg))))
#define NJSON_EXPAND2(arg) \
  NJSON_EXPAND3(NJSON_EXPAND3(NJSON_EXPAND3(NJSON_EXPAND3(arg))))
#define NJSON_EXPAND3(arg) \
  NJSON_EXPAND4(NJSON_EXPAND4(NJSON_EXPAND4(NJSON_EXPAND4(arg))))
#define NJSON_EXPAND4(arg) arg
#define NJSON_FOR_EACH(macro, ...) \
  __VA_OPT__(NJSON_EXPAND(NJSON_FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define NJSON_FOR_EACH_HELPER(macro, a1, ...) \
  macro(a1) __VA_OPT__(NJSON_FOR_EACH_AGAIN NJSON_PARENS(macro, __VA_ARGS__))
#define NJSON_FOR_EACH_AGAIN() NJSON_FOR_EACH_HELPER

#define SERIALIZE(field) j[#field] = t.field;
#define DESERIALIZE(field) j.at(#field).get_to(t.field);
#define DEFINE_TYPE_INTRUSIVE(Type, ...)                    \
  friend void to_json(nlohmann::json& j, const Type& t) {   \
    NJSON_FOR_EACH(SERIALIZE, __VA_ARGS__)                  \
  }                                                         \
  friend void from_json(const nlohmann::json& j, Type& t) { \
    NJSON_FOR_EACH(DESERIALIZE, __VA_ARGS__)                \
  }

typedef struct MultiCombo {
  std::vector<int> selected;
  std::vector<std::string> options;
  friend void to_json(nlohmann::json& j, const MultiCombo& c) {
    j = nlohmann::json::array({c.selected, c.options});
  }
  friend void from_json(const nlohmann::json& j, MultiCombo& c) {
    if (!j.is_array()) throw std::runtime_error("MultiCombo is not an array");
    j.at(0).get_to(c.selected);
    j.at(1).get_to(c.options);
  }
} MultiCombo;

typedef struct Combo {
  int selected;
  std::vector<std::string> options;
  friend void to_json(nlohmann::json& j, const Combo& c) {
    j = nlohmann::json::array({c.selected, c.options});
  }
  friend void from_json(const nlohmann::json& j, Combo& c) {
    if (!j.is_array()) throw std::runtime_error("Combo is not an array");
    j.at(0).get_to(c.selected);
    j.at(1).get_to(c.options);
  }
} Combo;

enum class KeybindType : uint8_t { AlwaysOn = 0, OnHotkey, Toggle, OffHotkey };
typedef struct Keybind {
 public:
  std::string key;
  KeybindType type;
  friend void to_json(nlohmann::json& j, const Keybind& c) {
    j = nlohmann::json::array({c.key, c.type});
  }
  friend void from_json(const nlohmann::json& j, Keybind& c) {
    if (!j.is_array()) throw std::runtime_error("Keybind is not an array");
    j.at(0).get_to(c.key);
    j.at(1).get_to(c.type);
  }
} Keybind;

enum class Hitbox : short { Head = 0, Chest, Stomach, Arms, Legs, Feet };

typedef struct RawConfig {
 public:
  bool aimbotRageEnabled;
  Keybind aimbotRageKey;
  Keybind aimbotForceSP;
  Color playerBWChamsC;
  MultiCombo aimbotRageMultipoint;
  Combo aimbotRageMultipointType;

  Keybind espActivationType;
  Color dormantC;
  bool teammates;
  bool boundingBox;
  Color boundingBoxC;
  bool healthBar;
  bool headDot;
  Color headDotC;
  bool showName;
  Color nameC;
  bool showFlags;
  bool weapText;
  bool weapIcon;
  bool showAmmo;
  bool showDst;
  bool skeleton;
  Color skeletonC;
  bool viewLines;
  Color viewLinesC;
  bool OOFArrow;
  Color OOFArrowC;

  bool footsteps;
  Color footstepsC;
  bool droppedWeapons;
  Color droppedWeaponsC;
  bool spike;
  Color spikeC;

  bool tbotEnabled;
  Keybind tbotKey;
  int tbotDelay;
  int tbotFireTime;
  MultiCombo tbotTargetHitbox;  // ["Head", "Chest", "Stomach", "Arms", "Legs",
                                // "Feet"],

  DEFINE_TYPE_INTRUSIVE(
      RawConfig, aimbotRageEnabled, aimbotRageKey, aimbotForceSP,
      playerBWChamsC, aimbotRageMultipoint, aimbotRageMultipointType,

      espActivationType, dormantC, teammates, boundingBox, boundingBoxC,
      healthBar, headDot, headDotC, showName, nameC, showFlags, weapText,
      weapIcon, showAmmo, showDst, skeleton, skeletonC, viewLines, viewLinesC,
      OOFArrow, OOFArrowC,

      footsteps, footstepsC, droppedWeapons, droppedWeaponsC, spike, spikeC,

      tbotEnabled, tbotKey, tbotDelay, tbotFireTime, tbotTargetHitbox);
  // friend void to_json(nlohmann::json& j, const RawConfig& c) {
  //   SERIALIZE(aimbotRageEnabled);
  //   SERIALIZE(aimbotRageKey);
  //   SERIALIZE(aimbotForceSP);
  //   SERIALIZE(playerBWChamsC);
  //   SERIALIZE(aimbotRageMultipoint);
  //   SERIALIZE(aimbotRageMultipointType);
  // }

  // friend void from_json(const nlohmann::json& j, RawConfig& c) {
  //   DESERIALIZE(aimbotRageEnabled);
  //   DESERIALIZE(aimbotRageKey);
  //   DESERIALIZE(aimbotForceSP);
  //   DESERIALIZE(playerBWChamsC);
  //   DESERIALIZE(aimbotRageMultipoint);
  //   DESERIALIZE(aimbotRageMultipointType);
  // }
} RawConfig;

typedef struct MenuData {
 public:
  int x;
  int y;
  int width;
  int height;
  int minWidth;
  int minHeight;
  int dpi;
  bool updatedTexture;
  DEFINE_TYPE_INTRUSIVE(MenuData, x, y, width, height, minWidth, minHeight, dpi,
                        updatedTexture);
} MenuData;
typedef struct GlobalConfig {
 public:
  MenuData menuData;
  std::string currentConfigName;
  bool LUAEnabled;
  bool LUAUnsafe;
  bool lockMenuLayout;
  Color menuColor;
  Keybind menuKey;
  Combo dpiScale;
  DEFINE_TYPE_INTRUSIVE(GlobalConfig, menuData, currentConfigName, LUAEnabled,
                        LUAUnsafe, lockMenuLayout, menuColor, menuKey,
                        dpiScale);
} GlobalConfig;

namespace keybinds {
  uint32_t strToVK(std::string vk);
}

namespace config {
  extern RawConfig c;
  extern GlobalConfig gc;
  void setup();
  void setConfig(const std::string& str);
  void setGlobalConfig(const std::string& str);
}  // namespace config

#endif /* CONFIG_CONFIG_H */
