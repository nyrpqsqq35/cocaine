#ifndef COCAINE_H
#define COCAINE_H

#define PHNT_VERSION PHNT_THRESHOLD
#undef UNICODE

// phnt_windows MUST be included before phnt
// clang-format off
#include <phnt_windows.h>
#include <phnt.h>
// clang-format on

#include <string.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#ifdef _DEBUG
  #define DbgLog(fmt, ...) printf(fmt, __VA_ARGS__)
#else
  #define DbgLog(fmt, ...)
#endif
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "config/config.h"

// For CEF
#undef GetNextSibling

#endif /* COCAINE_H */
