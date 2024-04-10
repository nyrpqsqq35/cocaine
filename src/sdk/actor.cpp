#include "actor.h"

#include <cocaine.h>

#include "sdk.h"

void Actor::fetchIds() {
  rd(actorPtr + offsets::actor_id, actorId);
  rd(actorPtr + offsets::fname_id, uniqueId);
}

#define PTRCHECKCONS(ptr) \
  (ptr == 0 || ptr <= 0x100000 || ptr >= 10000000000000000000ULL)
#define PTRCHECK(ptr, val) \
  if (PTRCHECKCONS(ptr)) return val

int Actor::fetch() {
  fetchIds();
  rd(actorPtr + offsets::current_mesh, meshPtr);
  PTRCHECK(meshPtr, 1);

  rd(actorPtr + offsets::root_component, rootComponentPtr);
  PTRCHECK(rootComponentPtr, 2);

  rd(actorPtr + offsets::damage_handler, damageHandlerPtr);
  rd(meshPtr + offsets::bone_array, boneArrayPtr);
  rd(meshPtr + offsets::bone_count, boneCount);

  rd(actorPtr + offsets::inventory, inventoryPtr);

  // if (this->isValidPlayer()) {
  // PTRCHECK(damageHandlerPtr, 3);
  // PTRCHECK(boneArrayPtr, 5);
  rd(actorPtr + offsets::player_state, playerStatePtr);
  if (playerStatePtr != 0) {
    rd(playerStatePtr + offsets::team_component, teamComponentPtr);
    if (!PTRCHECKCONS(teamComponentPtr)) {
      rd(teamComponentPtr + offsets::team_id, teamId);
    } else {
      teamId = 0;
    }
  }
  // }

  return 0;
}

bool Actor::shouldRefetch() { return true; }

bool Actor::recentlyRendered() {
  auto fLastSubmitTime = read<float>(meshPtr + offsets::last_render_time);
  auto fLastRenderTimeOnScreen = read<float>(meshPtr + offsets::last_sub_time);
  const float fVisionTick = 0.06f;
  return fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
}

bool Actor::isVisible() {
  auto fLastRenderTime = read<float>(meshPtr + offsets::last_render_time);
  auto fLastSubmitTime = read<float>(meshPtr + offsets::last_sub_time);
  const float fVisionTick = 0.06f;
  return fLastRenderTime + fVisionTick >= fLastSubmitTime;
}

bool Actor::isDormant() { return !(read<bool>(actorPtr + offsets::dormant)); }

uint64_t Actor::getNameHash() { return names::getActorHash(actorId); }
std::string Actor::getName() { return names::getActorName(actorId); }

std::string Actor::getAgent() { return names::getAgentName(actorId); }

uintptr_t Actor::getCurrentEquippable() {
  if (!inventoryPtr) return 0;
  return read<uintptr_t>(inventoryPtr + offsets::current_equippable);
}
// std::string Actor::getWeaponName()
float Actor::getHealth() {
  auto hp = read<float>(damageHandlerPtr + offsets::current_health);
  return hp;
}

float Actor::getMaxHealth() {
  auto max = read<float>(damageHandlerPtr + offsets::max_life);
  return max;
}

bool Actor::isAlive() {
  auto hp = getHealth();
  return hp > 0.f && hp <= 200.f;
}

bool Actor::isBot() { return isValidPlayer() && boneCount == 103; }

bool Actor::isValidPlayer() { return uniqueId == 18743553; }

struct Fuckname {
  int32_t meow;
  int32_t meow22;
  int32_t bitch;
};

struct FMeshBoneInfo {
  Fuckname name;
  int32_t parentIndex;
};

void readting(uintptr_t base, uintptr_t off) {
  auto deref = read<uintptr_t>(base + off);
  auto obj2 = read<int32_t>(deref);
  auto obj = read<int32_t>(deref + offsets::actor_id);

  auto name = names::getActorName(obj);
  auto name2 = names::getActorName(obj2);

  DbgLog("%llx = %llx, %s/%s (%i)\n", off, deref, name.c_str(), name2.c_str(),
         obj2);
}

Vector3 Actor::getBonePosition(int index) {
  auto size = sizeof(FTransform);
  auto fb = read<FTransform>(boneArrayPtr + (index * size));
  auto c2w = read<FTransform>(meshPtr + offsets::component_to_world);

  // auto SkeletalMesh = read<uintptr_t>(meshPtr + 0x590);
  // auto Skeleton = read<uintptr_t>(SkeletalMesh + 0x88);
  // auto refSkeleton = read<uintptr_t>(SkeletalMesh + 0x2C0);
  // auto RenderData = read<uintptr_t>(SkeletalMesh + 0x68);
  // auto LODRenderData = read<TArray<uintptr_t>>(RenderData);

  // readting(SkeletalMesh, 0x68);
  // readting(SkeletalMesh, 0x70);
  // readting(SkeletalMesh, 0x1d8);
  // readting(SkeletalMesh, 0x1e8);
  // readting(SkeletalMesh, 0x1f8);
  // readting(SkeletalMesh, 0x238);
  // readting(SkeletalMesh, 0x248);
  // readting(SkeletalMesh, 0x288);
  // readting(SkeletalMesh, 0x310);
  // readting(SkeletalMesh, 0x320);
  // readting(SkeletalMesh, 0x348);
  // readting(SkeletalMesh, 0x388);

  // uint8_t* a1 = new uint8_t[0x500];
  // uint8_t* a2 = new uint8_t[0x500];
  // uint8_t* a3 = new uint8_t[0x500];
  // uint8_t* a4 = new uint8_t[0x500];
  // driver->read_memory(valorantPid, read<uintptr_t>(SkeletalMesh + 0x68),
  //                     (uintptr_t)a1, 0x500);
  // driver->read_memory(valorantPid, read<uintptr_t>(SkeletalMesh + 0x70),
  //                     (uintptr_t)a2, 0x500);
  // driver->read_memory(valorantPid, read<uintptr_t>(SkeletalMesh + 0x78),
  //                     (uintptr_t)a3, 0x500);
  // driver->read_memory(valorantPid, read<uintptr_t>(SkeletalMesh + 0x80),
  //                     (uintptr_t)a4, 0x500);

  // // auto shit = read<uintptr_t>(SkeletalMesh + 0x80);

  // auto finalRefBoneInfo =
  //     read<TArray<FMeshBoneInfo>>(SkeletalMesh + 0x2C0 +
  //     0x10 + 0x10);

  // auto Sockets = read<TArray<uintptr_t>>(SkeletalMesh +
  // 0x378); auto VirtualBones =
  // read<TArray<FBoneNode>>(Skeleton + 0x40);
  // DbgLog("shit: %llx %llx\n", SkeletalMesh,
  // Skeleton); DbgLog("shitshit: %llx %i %i\n",
  // Sockets.data, Sockets.count,
  //        Sockets.max);
  // for (int i = 0; i < VirtualBones.count; ++i) {
  //   // auto MeshSocket = VirtualBones.at(i, false);
  //   // auto BoneName = read<int32_t>(MeshSocket + 0x00);
  //   auto shit = read<uintptr_t>(VirtualBones.data);
  //   DbgLog("Bone [%i] = %s\n", i,
  //   names::getActorName(shit).c_str());
  //   // DbgLog("sex [%i] = %s  /  %i\n", i,
  //   // names::getActorName(sex.Name).c_str(),
  //   //        sex.index);
  // }
  // auto actorId = read<int32_t>(meshPtr +
  // offsets::actor_id); auto name =
  // names::getActorName(actorId); DbgLog("Bone name =
  // %s\n", name.c_str());
  auto matrix =
      MatrixMultiplication(fb.ToMatrixWithScale(), c2w.ToMatrixWithScale());

  return Vector3(matrix._41, matrix._42, matrix._43);
}

Vector3 Actor::getPosition() {
  auto v3 = read<Vector3>(rootComponentPtr + offsets::relative_location);
  return v3;
}

FRotator Actor::getRotation() {
  auto rot = read<FRotator>(rootComponentPtr + offsets::relative_rotation);
  return rot;
}

Vector2 Actor::getScreenPosition() {
  return worldToScreen(getPosition(), sdk::camPos, sdk::camRot, sdk::width,
                       sdk::height, sdk::camFov);
}

bool Actor::isEnemy() { return teamId != sdk::localTeam || this->isBot(); }

ESPFlags Actor::getEspFlags() {
  ESPFlags flags;

  if (inventoryPtr) {
    auto equip = getCurrentEquippable();
    if (equip) {
      auto eqState = UStateMachineComponent::getStateHash(
          equip + 0xce0 /* AAresEquippable->EquippableStateMachine */);
      auto zmState = UStateMachineComponent::getStateHash(
          equip + 0x12b0 /* AGun_Zoomable_C->ZoomStateMachine */);
      flags.Reload = eqState == FNAME("ReloadState");
      flags.Zoom = zmState == FNAME("ZoomStateLevel1") ||
                   zmState == FNAME("ZoomStateLevel2");
    }

    for (int i = 0; i < 0x10; ++i) {
      auto pItemSlot =
          read<uintptr_t>(inventoryPtr + 0x188 /* UAresInventory->ItemSlots */ +
                          (i * sizeof(uintptr_t)));
      auto slotType = read<EAresItemSlot>(pItemSlot + 0x30);
      auto pItem = read<uintptr_t>(pItemSlot + 0x38);
      auto actorId = read<int32_t>(pItem + offsets::actor_id);
      auto hash = names::getActorHash(actorId);

      // Slot 7
      // BombEquippable_C

      // Slot 11
      // HeavyArmorItem_C (ABasicArmorItem_C)
      // LightArmorItem_C ?

      switch (slotType) {
        case EAresItemSlot::Level:
          flags.Spike = hash == FNAME("BombEquippable_C");
          break;
        case EAresItemSlot::GrenadeAbility:
          break;  // Abil C
        case EAresItemSlot::Ability1:
          break;  // Abil Q
        case EAresItemSlot::Ability2:
          break;  // Abil E
        case EAresItemSlot::Ultimate:
          break;  // Ulti X
        case EAresItemSlot::Armor:
          flags.HeavyArmor = hash == FNAME("HeavyArmorItem_C");
          flags.LightArmor =
              !flags.HeavyArmor && hash == FNAME("LightArmorItem_C");
          break;
        default:
          break;
      }
    }
  }

  auto anim = read<uintptr_t>(
      actorPtr + 0x12f0 /* ABasePlayerCharacter_C->AnimationState */);
  if (anim) {
    // auto actorId = read<int32_t>(anim + offsets::actor_id);
    // auto name = names::getActorName(actorId);
    // DbgLog("Anim class: %s\n", name.c_str());

    auto blindPercent = read<float>(
        anim + 0x100 /* UAnimationStateComponent->FAnimationStateParams */ +
        0x58 /* FAnimationStateParams.BlindPercent */);
    // auto blindAmount = read<float>(anim + 0x100 + 0x11c);
    flags.Blind = blindPercent <= 0.9f;
    // DbgLog("Blind from da anim state %.2f / %.2f\n", blindPercent,
    // blindAmount);
  }
  espFlags = flags;
  return flags;
}