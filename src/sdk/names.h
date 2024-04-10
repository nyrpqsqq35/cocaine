#ifndef SDK_NAMES_H
#define SDK_NAMES_H

#include <cstdint>
#include <string>

#include "xxh64.hpp"

#define XXH64_NAMES_SEED 0x3f835548ab8d9c47

constexpr int compiletime_strlen(const char* input) {
  int i = 0;
  while (input[i] != 0) {
    ++i;
  }
  return i;
}

#define FNAME(str) xxh64::hash(str, compiletime_strlen(str), XXH64_NAMES_SEED)

struct FNameEntryHandle {
  uint16_t bIsWide : 1;
  uint16_t Len : 15;
};
struct FNameEntryHeader {
  int32_t Block = 0;
  int32_t Offset = 0;
};

struct FNameEntry {
  int32_t ComparisonId;
  FNameEntryHandle Header;
  union {
    char AnsiName[1024];
    wchar_t WideName[1024];
  };

  const wchar_t* GetWideName() const { return WideName; }
  const char* GetAnsiName() const { return AnsiName; }
  bool IsWide() const { return Header.bIsWide; }
};
struct FNameEntryAllocator {
  enum { BlockSizeBytes = 4 * 65536 };
  mutable void* Lock;
  int32_t CurrentBlock;
  int32_t CurrentByteCursor;
  uintptr_t Blocks[8192];
};
struct FNamePool {
  enum { MaxENames = 512 };
  FNameEntryAllocator Entries;
  int32_t AnsiCount;
  int32_t WideCount;
};
struct FName {
  int32_t ComparisonIndex;
  int32_t Number;
};
struct XorStateAndKey {
  uintptr_t state[7];
  uintptr_t key;
};

struct OurTex;
namespace names {
  void setup();
  uint64_t getActorHash(int32_t actorId);
  std::string getActorName(int32_t actorId);

  std::string getAgentName(int32_t actorId);
  std::string getWeaponName(int32_t actorId);
  OurTex* getWeaponTexture(int32_t actorId);
  __attribute__((__format__(__printf__, 1, 2))) std::string format(
      const char* const fmt, ...);
}  // namespace names

#endif /* SDK_NAMES_H */
