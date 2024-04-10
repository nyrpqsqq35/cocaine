#include "hid.h"

#include <cocaine.h>
#include <hidapi/hidapi.h>

#define OPERATE_WITHOUT_HID 1
#ifdef OPERATE_WITHOUT_HID
  #define FAILCASE(str) return
#else
  #define FAILCASE(str) throw std::runtime_error(str)
#endif

namespace hid {
  PUserCmd cmd = nullptr;
  hid_device* handle = nullptr;
  void setup() {
    cmd = reinterpret_cast<PUserCmd>(new unsigned char[HID_INOUT_LEN]);

    int res = hid_init();
    if (res != 0) FAILCASE("hid_init() failed");

    std::string devPath;
    auto di = hid_enumerate(HID_VENDOR_ID, HID_PRODUCT_ID);
    auto orig = di;
    if (!di) FAILCASE("Could not locate HID device");
    do {
      if (di->vendor_id == HID_VENDOR_ID && di->product_id == HID_PRODUCT_ID &&
          di->usage_page == HID_USAGE_PAGE && di->usage == HID_USAGE) {
        DbgLog("Found our HID device %04x:%04x (%04x:%04x), %s\n",
               di->vendor_id, di->product_id, di->usage_page, di->usage,
               di->path);
        devPath = di->path;
      }
      di = di->next;
    } while (di != nullptr);
    hid_free_enumeration(orig);

    handle = hid_open_path(devPath.c_str());
    if (!handle) FAILCASE("Failed to open HID device");
  }
  void reset() {
    RtlSecureZeroMemory(cmd, HID_INOUT_LEN);
    cmd->reportId = HID_RID_INOUT;
    cmd->magic1 = USERCMD_MAGIC1;
    cmd->magic2 = USERCMD_MAGIC2;
  }
  void send() {
    if (!handle) return;
    int res =
        hid_write(handle, reinterpret_cast<unsigned char*>(cmd), HID_INOUT_LEN);
    if (res <= 0) throw std::runtime_error("Failed to write to HID device");
    DbgLog("Wrote %i bytes to HID device\n", res);
  }
  void shutdown() {
    if (handle) hid_close(handle);
#ifndef OPERATE_WITHOUT_HID
    hid_exit();
#endif
  }
}  // namespace hid