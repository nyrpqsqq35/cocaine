#ifndef HID_HID_H
#define HID_HID_H

#include <cstdint>

#include "hid_def.h"

namespace hid {
  extern PUserCmd cmd;

  void setup();
  void reset();
  void send();
  void shutdown();
}  // namespace hid

#endif /* HID_HID_H */
