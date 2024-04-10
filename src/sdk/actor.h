#ifndef SDK_ACTOR_H
#define SDK_ACTOR_H

#include <cstdint>
#include <string>

#include "vector.h"

enum class ActorType { None, Player, Spike, DroppedWeapon, Footstep };

typedef struct ESPFlags {
  union {
    uint32_t sex;
  };
  bool LightArmor : 1;
  bool HeavyArmor : 1;
  bool Zoom : 1;
  bool Blind : 1;
  bool Reload : 1;
  bool Spike : 1;
} ESPFlags;
static_assert(alignof(ESPFlags) == 4, "Die");

enum class EAresItemSlot : uint8_t {
  Primary = 0,
  Secondary = 1,
  Melee = 2,
  GrenadeAbility = 3,
  Ability1 = 4,
  Ability2 = 5,
  Passive = 6,
  Level = 7,
  Invisible = 8,
  Ultimate = 9,
  Unarmed = 10,
  Armor = 11,
  Backpack = 12,
  Totem = 13,
  PrimaryStorage = 14,
  SecondaryStorage = 15,
  Count = 16
};

class Actor {
 public:
  Actor(uintptr_t actorPtr) : actorPtr(actorPtr) {
    if (actorPtr) fetchIds();
  };
  uintptr_t actorPtr;
  uintptr_t damageHandlerPtr;

  uintptr_t playerStatePtr;
  uintptr_t teamComponentPtr;

  uintptr_t rootComponentPtr;
  uintptr_t meshPtr;
  uintptr_t boneArrayPtr;

  uintptr_t inventoryPtr;

  int boneCount;
  int teamId;
  uintptr_t uniqueId;
  int32_t actorId;
  ActorType actorType;
  int startFrame = 0;

  ESPFlags espFlags;

  void fetchIds();
  int fetch();

  bool shouldRefetch();

  bool recentlyRendered();
  bool isVisible();
  bool isDormant();

  uint64_t getNameHash();
  std::string getName();
  std::string getAgent();
  std::string getWeaponName();

  uintptr_t getCurrentEquippable();

  float getHealth();
  float getMaxHealth();
  bool isAlive();
  bool isEnemy();
  bool isBot();
  bool isValidPlayer();

  Vector3 getBonePosition(int index);
  Vector3 getPosition();
  FRotator getRotation();
  Vector2 getScreenPosition();

  // player:
  ESPFlags getEspFlags();

  friend inline bool operator==(const Actor& a, const Actor& b) {
    return a.actorPtr == b.actorPtr;
  }
};

#endif /* SDK_ACTOR_H */
