#ifndef SDK_RP_H
#define SDK_RP_H

#include <cstdint>

#include "driver.h"

extern uintptr_t valorantPid;
extern uintptr_t valorantBase;
extern HWND valorantHwnd;
template <typename T>
T read(uintptr_t address) {
  return driver->read<T>(valorantPid, address);
}

template <typename T>
void rd(uintptr_t address, T& out) {
  out = driver->read<T>(valorantPid, address);
}

template <typename T>
void write(uintptr_t address, T& buffer) {
  return driver->write<T>(valorantPid, address, buffer);
}

#endif /* SDK_RP_H */
