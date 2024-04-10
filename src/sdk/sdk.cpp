#include "sdk.h"

#include <TlHelp32.h>
#include <cocaine.h>
#include <xxhash.h>

uintptr_t valorantPid;
uintptr_t valorantBase;
HWND valorantHwnd;

#define XXH64SEED 0x4111588cba270a0e

#define VAL_HASH 0x71c77cc5080395aaULL

namespace {
  uintptr_t getValPid() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    if (!Process32First(snap, &pe))
      throw std::runtime_error("couldnt get first proc");
    while (Process32Next(snap, &pe)) {
      auto hash = XXH64(pe.szExeFile, strlen(pe.szExeFile), XXH64SEED);
      // DbgLog("%s - %llx\n", pe.szExeFile, hash);
      if (hash != VAL_HASH) continue;
      return pe.th32ProcessID;
    }
    return 0;
  }

  BOOL CALLBACK getValorantWindow(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == valorantPid) {
      valorantHwnd = hwnd;
      return FALSE;
    }
    return TRUE;
  }
}  // namespace

namespace sdk {
  int localTeam = 0;
  int width = 2560;
  int height = 1440;

  uintptr_t localCamera;
  Actor* lpa = nullptr;
  std::vector<Actor*> actors;
  std::vector<Actor*> players;

  uintptr_t World;
  uintptr_t GameInstance;
  uintptr_t PersistentLevel;
  uintptr_t GameState;

  uintptr_t LocalPlayerArray;
  uintptr_t LocalPlayer;
  uintptr_t LocalPlayerController;
  uintptr_t LocalPlayerPawn;
  uintptr_t LocalPlayerState;
  uintptr_t LocalPlayerTeamComp;

  Vector3 camPos;
  Vector3 camRot;
  float camFov;

  namespace ids {
    bool acquired = false;
    int32_t spikeId = 0;
    int32_t footstepId = 0;
  }  // namespace ids

  void setup() {
    bool s = driver->get_guarded_va(&driver->GuardedVA);
    if (!s) throw std::runtime_error("couldnt get GR VA");

    valorantPid = getValPid();
    valorantBase = driver->GetBaseAddress(valorantPid);
    names::setup();
  }
}  // namespace sdk