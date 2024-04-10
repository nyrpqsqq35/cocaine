#ifndef SDK_UE_H
#define SDK_UE_H
#include <cstdint>

#include "names.h"
#include "offsets.h"
#include "rp.h"
#include "vector.h"

typedef struct FCameraCacheEntry {
  float Timestamp;
  char _0[0xc];
  FMinimalViewInfo POV;
} FCameraCacheEntry;

template <typename T>
struct TArray {
  uintptr_t data;
  int32_t count;
  int32_t max;
  inline T at(int i, bool deref = true) {
    auto item = data + (i * sizeof(uintptr_t));
    if (deref) {
      auto ptr = read<uintptr_t>(item);
      // if (ptr == 0) return (T)(void*)0;
      return read<T>(ptr);
    } else {
      return read<T>(item);
    }
  }
  inline T operator[](int i) { return at(i); }
  inline T operator[](int i) const { return at(i); }
};

typedef struct FString {
  uintptr_t ptr;
  uintptr_t size;
} FString;

typedef struct APlayerState {
  char _p0[0x3d0];
  int32_t PlayerID;
  uint16_t Ping;
  char _p1[0x60];
  uintptr_t PawnPrivate;
  char _p2[0x78];
  FString PlayerNamePrivate;
} APlayerState;

// base @ 0x3d0
typedef struct AGameStateBase {
  char _p0[0x3d0];
  uintptr_t GameModeClass;
  uintptr_t AuthorityGameMode;
  uintptr_t SpectatorClass;
  TArray<APlayerState> PlayerArray;
} GameStateBase;

struct FStateContext {
  uintptr_t CurrentState;
  uintptr_t TransitionContext;
  float AuthStartWorldTime;
  // char pad_14[0x4];
};

class UStateMachineComponent {
 public:  // offsets
  const static uintptr_t AuthCurrentState = 0x1a8;

 public:
  static inline int32_t getCurrentStateId(uintptr_t sm) {
    auto auth = read<FStateContext>(sm + AuthCurrentState);
    return read<int32_t>(auth.CurrentState + offsets::actor_id);
  }
  static inline std::string getStateString(uintptr_t sm) {
    return names::getActorName(getCurrentStateId(sm));
  }
  static inline uint64_t getStateHash(uintptr_t sm) {
    return names::getActorHash(getCurrentStateId(sm));
  }
};

#endif /* SDK_UE_H */
