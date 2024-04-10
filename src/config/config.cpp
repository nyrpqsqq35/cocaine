#include "config.h"

#include <cocaine.h>

namespace config {
  RawConfig c;
  GlobalConfig gc;
  void setup() {}
  void setConfig(const std::string& str) {
    json cfg_ = json::parse(str);
    cfg_.get_to(c);

    printf("tbotKey = %s %hhu\n", c.tbotKey.key.c_str(), c.tbotKey.type);
    DbgLog("Set da config ting\n");
  }
  void setGlobalConfig(const std::string& str) {
    json cfg_ = json::parse(str);
    cfg_.get_to(gc);

    DbgLog("Set da globalconfig ting\n");
  }
}  // namespace config