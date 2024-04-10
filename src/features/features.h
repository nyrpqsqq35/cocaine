#ifndef FEATURES_FEATURES_H
#define FEATURES_FEATURES_H

#include <mutex>

#include "imgui/imgui_internal.h"

class Actor;
namespace features {
  namespace triggerbot {}
  namespace esp {
    extern std::mutex mutPlayers;
    extern std::mutex mutActors;
    extern ImDrawList* playerDrawList;
    extern ImDrawList* actorDrawList;

    void renderPlayer(ImDrawList* dl, Actor* player);
    void renderActor(ImDrawList* dl, Actor* actor);
    void render();
  }  // namespace esp
}  // namespace features

#endif /* FEATURES_FEATURES_H */
