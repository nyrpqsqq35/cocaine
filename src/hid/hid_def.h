#ifndef HID_HID_DEF_H
#define HID_HID_DEF_H

#include <cstdint>

#ifndef HID_NS
  #define HID_NS hid
  #define HID_VENDOR_ID 0x323E
  #define HID_PRODUCT_ID 0x0001
  #define HID_USAGE_PAGE 0xFF00
  #define HID_USAGE 0x0001
#else
  #define _HID_VENDOR_ID 0x323E
  #define _HID_PRODUCT_ID 0x0001
#endif

#define HID_RID_INOUT 2            // Report ID for generic inout
#define HID_INOUT_LEN 64           // Report size
#define USERCMD_MAGIC1 0x6e796161  // nyaa
#define USERCMD_MAGIC2 0x75777500  // uwu\0

namespace HID_NS {
  enum class Operation : uint8_t {
    None = 0,
    KeyPress,
    KeyRelease,
    KeyPressAndRelease,
    KeyReport
  };

#pragma pack(push, 1)
  typedef struct UserCmd {
      uint8_t reportId = HID_RID_INOUT;
      uint32_t magic1 = USERCMD_MAGIC1;
      uint32_t magic2 = USERCMD_MAGIC2;
      Operation operation = Operation::None;
      union {
          struct {
              char ch;
          } keyPress;
          struct {
          } keyRelease;
          struct {
              char ch;
              uint32_t delay;
          } keyPressAndRelease;
          struct {
              uint8_t modifier;
              uint8_t keycode[6];
          } keyReport;
      };
  } UserCmd, *PUserCmd;
#pragma pack(pop)
  static_assert(
      sizeof(UserCmd) <= HID_INOUT_LEN,
      "UserCmd is larger than the maximum allowed size for our HID message");
}  // namespace HID_NS

#endif /* HID_HID_DEF_H */
