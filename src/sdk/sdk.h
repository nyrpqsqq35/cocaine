#ifndef SDK_SDK_H
#define SDK_SDK_H

#include <vector>

#include "actor.h"
#include "driver.h"
#include "names.h"
#include "offsets.h"
#include "rp.h"
#include "ue.h"
#include "vector.h"

namespace sdk {
  extern int width;
  extern int height;
  extern uintptr_t localCamera;
  extern Actor* lpa;
  extern int localTeam;
  extern std::vector<Actor*> actors;
  extern std::vector<Actor*> players;

  extern Vector3 camPos;
  extern Vector3 camRot;
  extern float camFov;

  extern uintptr_t World;
  extern uintptr_t GameInstance;
  extern uintptr_t PersistentLevel;
  extern uintptr_t GameState;

  extern uintptr_t LocalPlayerArray;
  extern uintptr_t LocalPlayer;
  extern uintptr_t LocalPlayerController;
  extern uintptr_t LocalPlayerPawn;
  extern uintptr_t LocalPlayerState;
  extern uintptr_t LocalPlayerTeamComp;

  namespace ids {
    extern bool acquired;
    extern int32_t spikeId;
    extern int32_t footstepId;
  }  // namespace ids

  void setup();

  void iterateActors(uintptr_t actorArray, int actorCount,
                     std::function<void(Actor*)> cb = nullptr);
  void iteratePlayers(TArray<APlayerState>* playerArray,
                      std::function<void(Actor*)> cb = nullptr);
}  // namespace sdk
#endif /* SDK_SDK_H */
