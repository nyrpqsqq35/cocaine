#include "bmain.h"

#include <cocaine.h>
#include <imgui.h>

#include <fstream>
#include <thread>

#include "config/keybinds.h"
#include "features/features.h"
#include "hid/hid.h"
#include "render/render.h"
#include "sdk/names.h"
#include "sdk/rev.h"
#include "sdk/sdk.h"

void setupDriver() {
  driver = new Driver();
  if (!driver->initialize()) throw std::runtime_error("D not init");

  uintptr_t base = driver->GetBaseAddress(driver->currentProcessId);
  DbgLog("-> baseAddr = 0x%llx\n", base);
  if (base == 0) throw std::runtime_error("Couldn't get our base");

  int cmpVar = 0x69420;
  NTSTATUS status = 0;
  int drvVar =
      driver->read<int>(driver->currentProcessId, (uintptr_t)&cmpVar, &status);
  DbgLog("-> acq var = 0x%05x\n-> local var = 0x%05x\n", drvVar, cmpVar);
  if (drvVar != cmpVar) throw std::runtime_error("Var check failed");
}

#define LOG(ptr) DbgLog("  > " #ptr " = %llx\n", ptr)
#define PTRCHECK(ptr, breakOut)                                  \
  if (ptr == 0) {                                                \
    DbgLog("Ptr " #ptr " is nullptr, ret early\n");              \
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); \
    actorsLock.lock();                                           \
    sdk::actors.clear();                                         \
    actorsLock.unlock();                                         \
    playersLock.lock();                                          \
    sdk::players.clear();                                        \
    playersLock.unlock();                                        \
    breakOut;                                                    \
  }

namespace {
  std::thread thrMemory;
  std::thread thrPlayer;
  std::thread thrTrigger;
  std::thread thrKeybind;
}  // namespace

std::mutex actorsLock;
std::mutex playersLock;

bool checkBone(float dist, int cx, int cy, Vector2 bonePos) {
  auto dx = std::abs(bonePos.x - cx);
  auto dy = std::abs(bonePos.y - cy);
  auto multi = std::max(10.f, dist * 0.0005f);
  return dx < multi && dy < multi;
}

void fire() {
  static bool once = []() {
    hid::reset();
    hid::cmd->operation = hid::Operation::KeyPressAndRelease;
    hid::cmd->keyPressAndRelease.ch = 'h';
    hid::cmd->keyPressAndRelease.delay = config::c.tbotFireTime;
    return true;
  }();
  hid::send();
}

void sdk::iterateActors(uintptr_t actorArray, int actorCount,
                        std::function<void(Actor*)> cb) {
  static int lastActorCount = 0;
  if (lastActorCount == actorCount) {
    actorsLock.lock();
    for (auto& a : actors) {
      a->fetch();
      if (cb) cb(a);
    }
    actorsLock.unlock();
  } else {
    uintptr_t actorPtr = 0;
    int fetchRes = 0;
    for (int i = 0; i < actorCount; ++i) {
      actorPtr = read<uintptr_t>(actorArray + (i * sizeof(uintptr_t)));
      if (actorPtr == 0) continue;

      Actor* a;

      actorsLock.lock();
      auto it = std::find_if(
          actors.begin(), actors.end(),
          [actorPtr](const Actor* act) { return actorPtr == act->actorPtr; });
      actorsLock.unlock();

      if (it == actors.end()) {
        a = new Actor(actorPtr);
        a->fetchIds();
        auto hash = a->getNameHash();

        switch (hash) {
          case FNAME("FXC_Footstep_C"):
            a->actorType = ActorType::Footstep;
            break;
          case FNAME("TimedBomb_C"):
            a->actorType = ActorType::Spike;
            break;
          default:
            continue;
        }
        actorsLock.lock();
        fetchRes = a->fetch();
        actors.emplace_back(a);
        actorsLock.unlock();

      } else {
        a = (*it);
        fetchRes = a->fetch();
        if (fetchRes != 0) {
          actorsLock.lock();
          actors.erase(it);
          actorsLock.unlock();

          continue;
        }
      }
      if (cb) cb(a);
    }
    lastActorCount = actorCount;
  }
}

void sdk::iteratePlayers(TArray<APlayerState>* playerArray,
                         std::function<void(Actor*)> cb) {
  playersLock.lock();
  for (int i = 0; i < playerArray->count; ++i) {
    auto aps = playerArray->at(i);
    uintptr_t fuck =
        read<uintptr_t>(playerArray->data + (i * sizeof(uintptr_t)));

    uintptr_t actorPtr = aps.PawnPrivate;
    if (!actorPtr) {
      actorPtr = read<uintptr_t>(fuck + 0x900);
      if (!actorPtr) {
        actorPtr = read<uintptr_t>(fuck + 0x908);
        if (!actorPtr) {
          actorPtr = read<uintptr_t>(fuck + 0x910);
          if (!actorPtr) {
            continue;
          }
        }
      }
    };

    Actor* a;
    auto it = std::find_if(
        players.begin(), players.end(),
        [actorPtr](const Actor* act) { return actorPtr == act->actorPtr; });
    if (it == players.end()) {
      a = new Actor(aps.PawnPrivate);
      a->fetch();
      a->actorType = ActorType::Player;
      players.emplace_back(a);
    } else {
      a = (*it);
      a->fetch();
    }
    if (cb) cb(a);
  }
  playersLock.unlock();
}

void playerThread() {
  while (true) {
    playersLock.lock();
    auto tp = sdk::players;
    playersLock.unlock();
    for (const auto& p : tp) {
      if (p) p->getEspFlags();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void triggerThread() {
  while (true) {
    if (!keybinds::triggerbot) continue;

    playersLock.lock();
    auto tplayers = sdk::players;
    playersLock.unlock();

    int cx = sdk::width / 2;
    int cy = sdk::height / 2;
    Vector3 vec;
    bool matchesBone = false;
    float dist = 0.f;

    for (auto& player : tplayers) {
      if (!player->isAlive()) continue;
      if (!player->isEnemy()) continue;

      vec = player->getBonePosition(8);
      dist = sdk::camPos.Distance(vec);

      for (int hb : config::c.tbotTargetHitbox.selected) {
        if (matchesBone) break;
        switch (hb) {
          case 0:  // Head
            matchesBone = checkBone(dist, cx, cy, W2S(vec));
            break;
          case 1:  // Chest
            vec = player->getBonePosition(6);
            matchesBone = checkBone(dist, cx, cy, W2S(vec));
            break;
          case 2:  // Stomach
            vec = player->getBonePosition(4);
            matchesBone = checkBone(dist, cx, cy, W2S(vec));
            break;
          case 3:  // Arms
          case 4:  // Legs
          case 5:  // Feet
            vec = player->getBonePosition(3);
            matchesBone = checkBone(dist, cx, cy, W2S(vec));
            break;
        }
      }

      if (matchesBone) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(config::c.tbotDelay));
        fire();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(config::c.tbotFireTime));
        break;
      }
    }
  }
}

// FLAGS
// B (C4)
// HK /

void memoryThread() {
  static uintptr_t lastController = 0;
  static uintptr_t lastPawn = 0;

  features::esp::actorDrawList = new ImDrawList(ImGui::GetDrawListSharedData());
  sdk::lpa = new Actor(0);
  auto adl = new ImDrawList(ImGui::GetDrawListSharedData());

  while (true) {
    // uint64_t worldKey = 0;
    // struct State {
    //   uint64_t Keys[7];
    // } state;
    // rd(valorantBase + offsets::uworld_key, worldKey);
    // rd(valorantBase + offsets::uworld_state, state);

    // auto decryptedWorld = decrypt_uworld(worldKey, (uintptr_t*)&state);
    rd(0x10000000060, sdk::World);
    // DbgLog("decryptedWorld = %llx\n", decryptedWorld);
    DbgLog("sdk::World = %llx\n", sdk::World);
    rd(sdk::World + offsets::owning_game_instance, sdk::GameInstance);
    PTRCHECK(sdk::World, continue);

    PTRCHECK(sdk::GameInstance, continue);

    rd(sdk::World + offsets::persistent_level, sdk::PersistentLevel);
    PTRCHECK(sdk::PersistentLevel, continue);

    rd(sdk::World + offsets::game_state, sdk::GameState);
    PTRCHECK(sdk::GameState, continue);

    rd(sdk::GameInstance + offsets::local_players, sdk::LocalPlayerArray);
    PTRCHECK(sdk::LocalPlayerArray, continue);

    rd(sdk::LocalPlayerArray, sdk::LocalPlayer);
    PTRCHECK(sdk::LocalPlayer, continue);

    rd(sdk::LocalPlayer + offsets::player_controller,
       sdk::LocalPlayerController);
    PTRCHECK(sdk::LocalPlayerController, continue);

    rd(sdk::LocalPlayerController + offsets::acknowledged_pawn,
       sdk::LocalPlayerPawn);
    PTRCHECK(sdk::LocalPlayerPawn, continue);

    rd(sdk::LocalPlayerPawn + offsets::player_state, sdk::LocalPlayerState);
    PTRCHECK(sdk::LocalPlayerState, continue);

    rd(sdk::LocalPlayerState + offsets::team_component,
       sdk::LocalPlayerTeamComp);
    PTRCHECK(sdk::LocalPlayerTeamComp, continue);

    rd(sdk::LocalPlayerController + offsets::player_camera, sdk::localCamera);
    rd(sdk::LocalPlayerTeamComp + offsets::team_id, sdk::localTeam);

    sdk::lpa->actorPtr = sdk::LocalPlayerPawn;
    sdk::lpa->fetchIds();
    sdk::lpa->fetch();

    // if (lastController != sdk::LocalPlayerController ||
    //     lastPawn != sdk::LocalPlayerPawn) {
    //   actorsLock.lock();
    //   sdk::actors.clear();
    //   actorsLock.unlock();
    //   lastController = sdk::LocalPlayerController;
    //   lastPawn = sdk::LocalPlayerPawn;
    // }

    // sdk::lpa->getEspFlags();

    // auto actorArray =
    //     read<uintptr_t>(sdk::PersistentLevel + offsets::actor_array);
    // PTRCHECK(actorArray, continue);
    // int actorCount = read<int>(sdk::PersistentLevel + offsets::actor_count);

    // adl->_ResetForNewFrame();
    // adl->PushTextureID(ImGui::GetIO().Fonts->TexID);
    // adl->PushClipRectFullScreen();

    // sdk::iterateActors(actorArray, actorCount,
    //                    [adl](Actor* a) { features::esp::renderActor(adl, a);
    //                    });

    // features::esp::mutActors.lock();
    // delete features::esp::actorDrawList;
    // features::esp::actorDrawList = adl->CloneOutput();
    // features::esp::mutActors.unlock();

    // std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void bmain() {
  setupDriver();
  sdk::setup();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  DbgLog("vpid = %llx, vbase = %llx\n", valorantPid, valorantBase);
  if (valorantPid == 0 || valorantBase == 0)
    throw std::runtime_error("Valoroant PID or base is 0");

  thrMemory = std::thread(memoryThread);
  thrTrigger = std::thread(triggerThread);
  thrPlayer = std::thread(playerThread);
  thrKeybind = std::thread(keybinds::keybindThread);
}
