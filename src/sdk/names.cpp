#include "names.h"

#include <cocaine.h>

#include <fstream>

#include "render/rimg.h"
#include "sdk.h"
#include "xxh64.hpp"

// #define COMPARISON(name_, value_) \
//   if (name == name_) return value_
#define COMPARISON(name_, value_) \
  case (FNAME(name_)):            \
    return value_

namespace names {
  // Acquired by getting 0th FNamePool entry (None) and deriving key from it
  static union {
    uint32_t nameXorKey_u32;
    uint8_t nameXorKey[4];
  };

  template <typename T>
  inline constexpr T Align(T Val, uint64_t Alignment) {
    return (T)(((uint64_t)Val + Alignment - 1) & ~(Alignment - 1));
  }
  __attribute__((__format__(__printf__, 1, 2))) std::string format(
      const char* const fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    auto neededSize = vsnprintf(nullptr, 0, fmt, ap) + 1;
    char* buf = new char[neededSize];
    vsprintf_s(buf, neededSize, fmt, ap);
    std::string str(buf);

    delete[] buf;
    va_end(ap);
    return str;
  }

  void decodeEntry(uintptr_t entry_, std::string& out) {
    auto entry = read<FNameEntry>(entry_);
    auto length = entry.Header.Len;
    if (!length) {
      out = "meooow uwu";
      return;
      // throw std::runtime_error("FName id is bad");
    }
    if (length >= 480) {
      out = "Nyaaa~";
      return;
    }
    if ((entry_ == 0 || entry_ <= 0x100000 ||
         entry_ >= 1000000000000000000ULL)) {
      out = "meooow~";
      return;
    }
    if (entry.Header.bIsWide) {
      for (uint16_t i = 0; i < length; ++i) {
        entry.WideName[i] ^= length ^ nameXorKey[i & 3];
      }
      std::wstring ws(entry.WideName, length);
      out = std::string(ws.begin(), ws.end());
    } else {
      for (uint16_t i = 0; i < length; ++i) {
        entry.AnsiName[i] ^= length ^ nameXorKey[i & 3];
      }
      // printf("%.*s\n", length, entry.WideName);
      out = std::string(entry.AnsiName, length);
    }
  }

  std::string decodeEntry(uintptr_t entry) {
    std::string out;
    decodeEntry(entry, out);
    return out;
  }

  void dumpFNames(bool getKeys = false) {
    auto namePoolDataAddr = valorantBase + offsets::fname_pool;
    // LOG(namePoolDataAddr);

    auto namePoolData = read<FNamePool>(namePoolDataAddr);
    DbgLog("Entries.CurrentBlock %i\nEntries.CurrentByteCursr %i\n",
           namePoolData.Entries.CurrentBlock,
           namePoolData.Entries.CurrentByteCursor);

    // auto xorKeys = getXorKey();

    std::ofstream out("./fnames.txt");
    for (int iBlock = 0; iBlock < namePoolData.Entries.CurrentBlock; ++iBlock) {
      auto blockPtr = namePoolData.Entries.Blocks[iBlock];
      // DbgLog("Blocks[%i] = %llx\n", iBlock, blockPtr);
      out << format("Blocks[%i] = %llx\n", iBlock, blockPtr);

      auto nameOff = offsetof(FNameEntry, AnsiName);
      auto end = blockPtr + FNameEntryAllocator::BlockSizeBytes - nameOff;
      // DbgLog("  >> End = %llx\n", end);
      out << format("  >> End = %llx\n", end);

      auto it = blockPtr;

      while (it < end) {
        auto entry = read<FNameEntry>(it);
        auto length = entry.Header.Len;
        if (!length) {
          break;
        } else {
          // Fuck ur decrypt Riot
          if (entry.ComparisonId == 0 && length == 4) {
            nameXorKey[0] = entry.AnsiName[0] ^ length ^ 'N';
            nameXorKey[1] = entry.AnsiName[1] ^ length ^ 'o';
            nameXorKey[2] = entry.AnsiName[2] ^ length ^ 'n';
            nameXorKey[3] = entry.AnsiName[3] ^ length ^ 'e';
            if (getKeys) {
              iBlock = namePoolData.Entries.CurrentBlock + 1;
              break;
            }
          }
          auto str = decodeEntry(it);
          out << format("[%i] <%llx> - %i %.*s\n", entry.ComparisonId, it,
                        entry.Header.Len, length, str.c_str());

          // if (entry.Header.bIsWide) {
          //   for (uint16_t i = 0; i < length; ++i) {
          //     entry.WideName[i] ^= length ^ xorKeys[i & 3];
          //   }
          //   // printf("%.*ls\n", length, entry.WideName);
          //   out << format("%.*ls\n", length, entry.WideName);
          // } else {
          //   for (uint16_t i = 0; i < length; ++i) {
          //     entry.AnsiName[i] ^= length ^ xorKeys[i & 3];
          //   }
          //   // printf("%.*s\n", length, entry.AnsiName);
          //   out << format("%.*s\n", length, entry.AnsiName);
          // }
        }
        it += Align(nameOff + length * (entry.Header.bIsWide ? sizeof(wchar_t)
                                                             : sizeof(char)),
                    alignof(FNameEntry));
      }
    }
    out.close();
  }

  void setup() {
    // Need to setup nameXorKey
    // getXorKey();
    dumpFNames(true);
  }

#define LOG(ptr) printf(" >" #ptr " %llx\n", ptr);

  // Can we cache these?
  // Do they change between matches?
  std::string getActorName(int32_t actorId) {
    auto fNamePool = valorantBase + offsets::fname_pool;
    auto chunkOffset = (unsigned int)((int)(actorId) >> 16);
    auto nameOffset = (unsigned short)(actorId);
    if (nameOffset == 0) return "die";

    auto namePoolChunk = read<uintptr_t>(fNamePool + ((chunkOffset + 2) * 8));
    if (namePoolChunk == 0 || namePoolChunk < 0x1000000) return "meow";
    // throw std::runtime_error("Definitely invalid entry");
    // DbgLog("chunkOffset = %i\nnameOffset = %i\n", chunkOffset, nameOffset);
    // LOG(namePoolChunk);
    auto entryOffset = namePoolChunk + (unsigned long)(4 * nameOffset);
    // LOG(entryOffset);
    if (entryOffset < 0x1000000) return "nya";
    // throw std::runtime_error("Probable invalid entry");
    return decodeEntry(entryOffset);
  }
  uint64_t getActorHash(int32_t actorId) {
    auto name = getActorName(actorId);
    return xxh64::hash(name.c_str(), name.size(), XXH64_NAMES_SEED);
  }

  /*
  https://valorant-api.com/v1/agents
    JSON.parse($0.innerText)
      .data.map(i=>`COMPARISON("${i.developerName}_PC_C", "${i.displayName}");`)
      .join('\n')
  */
  std::string getAgentName(int32_t actorId) {
    auto hash = getActorHash(actorId);
    switch (hash) {
      COMPARISON("BountyHunter_PC_C", "Fade");
      COMPARISON("Breach_PC_C", "Breach");
      COMPARISON("Clay_PC_C", "Raze");
      COMPARISON("Deadeye_PC_C", "Chamber");
      COMPARISON("Grenadier_PC_C", "KAY/O");
      COMPARISON("Guide_PC_C", "Skye");
      COMPARISON("Gumshoe_PC_C", "Cypher");
      COMPARISON("Hunter_NPE_PC_C", "Sova");
      COMPARISON("Hunter_PC_C", "Sova");
      COMPARISON("Killjoy_PC_C", "Killjoy");
      COMPARISON("Mage_PC_C", "Harbor");
      COMPARISON("Pandemic_PC_C", "Viper");
      COMPARISON("Phoenix_PC_C", "Phoenix");
      COMPARISON("Rift_PC_C", "Astra");
      COMPARISON("Sarge_PC_C", "Brimstone");
      COMPARISON("Sprinter_PC_C", "Neon");
      COMPARISON("Stealth_PC_C", "Yoru");
      COMPARISON("Thorne_PC_C", "Sage");
      COMPARISON("Vampire_PC_C", "Reyna");
      COMPARISON("Wraith_PC_C", "Omen");
      COMPARISON("Wushu_PC_C", "Jett");
      COMPARISON("AggroBot_PC_C", "Gekko");

      COMPARISON("Rift_TargetingForm_PC_C", "Astra (Ultimate Form)");
      COMPARISON("TrainingBot_PC_C", "Training Bot");
      COMPARISON("Pawn_TrainingBot_DanceHall_Easy_C", "Training Bot");
      COMPARISON("Pawn_TrainingBot_Defuse_Ultimate_C", "Training Bot");
      default:
        return "?";
    }
  }
  std::string getWeaponName(int32_t actorId) {
    auto hash = getActorHash(actorId);
    switch (hash) {
      COMPARISON("Ability_Melee_Base_C", "Knife");

      COMPARISON("TrainingBotBasePistol_C", "Classic");
      COMPARISON("BasePistol_C", "Classic");
      COMPARISON("SawedOffShotgun_C", "Shorty");
      COMPARISON("AutomaticPistol_C", "Frenzy");
      COMPARISON("LugerPistol_C", "Ghost");
      COMPARISON("RevolverPistol_C", "Sheriff");

      COMPARISON("Vector_C", "Stinger");
      COMPARISON("SubMachineGun_MP5_C", "Spectre");

      COMPARISON("PumpShotgun_C", "Bucky");
      COMPARISON("AutomaticShotgun_C", "Judge");

      COMPARISON("AssaultRifle_Burst_C", "Bulldog");
      COMPARISON("DMR_C", "Guardian");

      COMPARISON("AssaultRifle_ACR_C", "Phantom");
      COMPARISON("AssaultRifle_AK_C", "Vandal");

      COMPARISON("LeverSniperRifle_C", "Marshal");
      COMPARISON("BoltSniper_C", "Operator");

      COMPARISON("LightMachineGun_C", "Ares");
      COMPARISON("HeavyMachineGun_C", "Odin");

      COMPARISON("BombEquippable_C", "Spike");

      // Astra
      COMPARISON("Ability_Rift_4_BlackHole_C", "Gravity Well");
      COMPARISON("Ability_Rift_Q_FlashBurst_C", "Nova Pulse");
      COMPARISON("Ability_Rift_E_TransformRRift_Smoke_C", "Nebula");
      COMPARISON("Ability_Rift_X_WorldTargetingStart_C", "Astral Form");
      COMPARISON("Ability_Rift_X_PlaceMarkers_WorldTargeting_GlobalWall_C",
                 "Astral Form");

      // Breach
      COMPARISON("Ability_Breach_4_FusionBlast_C", "Aftershock");
      COMPARISON("Ability_Breach_Q_Flash_C", "Flashpoint");
      COMPARISON("Ability_Breach_E_Fissure_C", "Fault Line");
      COMPARISON("Ability_Breach_X_Shockwave_C", "Rolling Thunder");

      // Brimstone
      COMPARISON("Ability_Sarge_E_SpeedStim_C", "Stim Beacon");
      COMPARISON("Ability_Sarge_Q_Molotov_Production_C", "Incendiary");
      COMPARISON("Ability_Sarge_4_MapTargetSmoke_Production_C", "Sky Smoke");
      COMPARISON("Ability_Sarge_X_OrbitalStrike_C", "Orbital Strike");

      // Chamber
      COMPARISON("Ability_Deadeye_4_Trap_C", "Trademark");
      COMPARISON("Gun_Deadeye_Q_Pistol_C", "Headhunter");
      COMPARISON("Ability_Deadeye_E_Teleporter_Tethers_C", "Rendezvous");
      COMPARISON("Gun_Deadeye_X_Giantslayer_Prototype_FireRatePrototype_C",
                 "Tour de Force");

      // Cypher
      COMPARISON("Ability_Gumshoe_E_TripWire_C", "Trapwire");
      COMPARISON("Ability_Gumshoe_4_CageTrap_C", "Cyber Cage");
      COMPARISON("Ability_Gumshoe_Q_Camera_C", "Spycam");
      COMPARISON("Ability_Gumshoe_Q_Camera_Dart_C", "Spycam (*)");
      // COMPARISON("", "Cypher ult");

      // Fade
      COMPARISON("Ability_BountyHunter_4_WolfHoundBendable_C", "Prowler");
      COMPARISON("Ability_Q_BountyHunter_TetherDivebomb_C", "Seize");
      COMPARISON("Ability_E_BountyHunter_ReconDivebomb_C", "Haunt");
      COMPARISON("Ability_BountyHunter_X_WaveForm_C", "Nightfall");

      // Gekko
      COMPARISON("Ability_Aggrobot_C_ExplodeyPatch_C", "Mosh's Pit");
      COMPARISON("Ability_Q_Aggrobot_SeekerNade_C", "Wingman");
      COMPARISON("Ability_E_Aggrobot_DiscTurret_C", "Dizzy");
      COMPARISON("Ability_Aggrobot_X_RollyExplosion_C", "Thrash");

      // Harbor
      COMPARISON("Ability_Mage_4_Wave_C", "Cascade");
      COMPARISON("Ability_Mage_Q_ShieldNade_C", "Cove");
      COMPARISON("Ability_Mage_E_Wall_C", "High Tide");
      COMPARISON("Ability_Mage_X_Storm_C", "Reckoning");

      // Jett
      COMPARISON("Ability_Wushu_4_Smoke_C", "Cloudburst");
      COMPARISON("Ability_Wushu_Q_CycloneBoost_C", "Updraft");
      COMPARISON("Ability_Wushu_E_Dash_Production_C", "Tailwind");
      COMPARISON("Ability_Wushu_X_Dagger_Production_C", "Blade Storm");

      // KAY/O
      COMPARISON("Ability_Grenadier_Q_BasicSemtex_C", "FRAG/MENT");
      COMPARISON("Ability_Grenadier_C_Flash_C", "FLASH/DRIVE");
      COMPARISON("Ability_E_Grenadier_EMPKnife_C", "ZERO/POINT");
      COMPARISON("Ability_Grenadier_X_OverloadPulse_C", "NULL/CMD");

      // Killjoy
      COMPARISON("Ability_Killjoy_4_RemoteBees_MultiDetonate_C", "Nanoswarm");
      COMPARISON("Ability_Killjoy_Q_Alarmbot_C", "Alarmbot");
      COMPARISON("Ability_Killjoy_E_Turret_C", "Turret");
      COMPARISON("Ability_Killjoy_X_Bomb_C", "Lockdown");

      // Neon
      COMPARISON("Ability_Sprinter_4_Tunnel_Production_C", "Fast Lane");
      COMPARISON("Ability_Sprinter_Q_GroundStrike_Production_C", "Relay Bolt");
      COMPARISON("Ability_Sprinter_E_Sprint_Production_C", "High Gear");
      COMPARISON("Gun_Sprinter_X_HeavyLightningGun_Production_C", "Overdrive");

      // Omen
      COMPARISON("Ability_Wraith_E_ShortTeleport_C", "Shrouded Step");
      COMPARISON("Ability_Wraith_Q_NearsightMissile_C", "Paranoia");
      COMPARISON("Ability_Wraith_4_Smoke_C", "Dark Cover");
      COMPARISON("Ability_Wraith_X_GlobalTeleport_C", "From the Shadows");

      // Phoenix
      COMPARISON("Ability_Phoenix_Q_FireballWall_Production_C", "Blaze");
      COMPARISON("Ability_Phoenix_E_FlareCurve_Production_C", "Curveball");
      COMPARISON("Ability_Phoenix_4_Molotov_PatchPrototype_C", "Hot Hands");
      COMPARISON("Ability_Phoenix_X_SelfRes_Production_C", "Run it Back");

      // Raze
      COMPARISON("Ability_Clay_E_Boomba_C", "Boom Bot");
      COMPARISON("Ability_Clay_Q_Satchel_C", "Blast Pack");
      COMPARISON("Ability_Clay_4_ClusterGrenade_C", "Paint Shells");
      COMPARISON("Ability_Clay_X_RocketLauncher_C", "Showstopper");

      // Reyna
      COMPARISON("Ability_Vampire_4_NearsightAoE_C", "Leer");
      // COMPARISON("", "Devour");
      // COMPARISON("", "Dismiss");
      COMPARISON("Ability_Vampire_X_Frenzy_C", "Empress");

      // Sage
      COMPARISON("Ability_Thorne_E_Wall_Fortifying_C", "Barrier Orb");
      COMPARISON("Ability_Thorne_4_SlowField_Production_C", "Slow Orb");
      COMPARISON("Ability_Thorne_Q_Heal_Production_New_C", "Healing Orb");
      COMPARISON("Ability_Thorne_X_Resurrect_Production_C", "Resurrection");

      // Skye
      COMPARISON("Ability_Guide_4_Heal_C", "Regrowth");
      COMPARISON("Ability_Guide_Q_PossessableScout_C", "Trailblazer");
      COMPARISON("Ability_Guide_Q_PossessableScout_ScoutAbilities_C",
                 "Trailblazer (*)");  // Controlling her dog
      COMPARISON("Ability_Guide_E_HawkFlash_C", "Guiding Light");
      COMPARISON("Ability_Guide_X_Pack_C", "Seekers");

      // Sova
      COMPARISON("Ability_Hunter_E_DeployDrone_C", "Owl Drone");
      COMPARISON("Ability_Hunter_E_Drone_Abilities_C",
                 "Owl Drone (*)");  // Controlling owl drone
      COMPARISON("Ability_Hunter_4_BoltExplosive_C", "Shock Bolt");
      COMPARISON("Ability_Hunter_Q_RevealBolt_Signature_C", "Recon Bult");
      COMPARISON("Ability_Hunter_X_LaserMulti_C", "Hunter's Fury");

      // Viper
      COMPARISON("Ability_Pandemic_Q_AcidGrenade_C", "Snake Bite");
      COMPARISON("Ability_Pandemic_4_SmokeGrenade_C", "Poison Cloud");
      COMPARISON("Ability_Pandemic_E_SmokeScreen_C", "Toxic Screen");
      COMPARISON("Ability_Pandemic_X_SmokeSite_New_C", "Viper's Pit");

      // Yoru
      COMPARISON("Ability_Stealth_4_Decoy_C", "Fakeout");
      COMPARISON("Ability_Stealth_Q_BounceFlash_C", "Blindside");
      COMPARISON("Ability_Stealth_E_Teleport_C", "Gatecrash");
      COMPARISON("Ability_Stealth_X_Cloak_Equip_C", "Dimensional Drift");
      default:
        return "?";
    }
  }
  OurTex* getWeaponTexture(int32_t actorId) {
    auto hash = getActorHash(actorId);
    switch (hash) {
      COMPARISON("Ability_Melee_Base_C", &render::tex::melee);

      COMPARISON("BasePistol_C", &render::tex::classic);
      COMPARISON("SawedOffShotgun_C", &render::tex::shorty);
      COMPARISON("AutomaticPistol_C", &render::tex::frenzy);
      COMPARISON("LugerPistol_C", &render::tex::ghost);
      COMPARISON("RevolverPistol_C", &render::tex::sheriff);

      COMPARISON("Vector_C", &render::tex::stinger);
      COMPARISON("SubMachineGun_MP5_C", &render::tex::spectre);

      COMPARISON("PumpShotgun_C", &render::tex::bucky);
      COMPARISON("AutomaticShotgun_C", &render::tex::judge);

      COMPARISON("AssaultRifle_Burst_C", &render::tex::bulldog);
      COMPARISON("DMR_C", &render::tex::guardian);

      COMPARISON("AssaultRifle_ACR_C", &render::tex::phantom);
      COMPARISON("AssaultRifle_AK_C", &render::tex::vandal);

      COMPARISON("LeverSniperRifle_C", &render::tex::marshal);
      COMPARISON("BoltSniper_C", &render::tex::oper8or);

      COMPARISON("LightMachineGun_C", &render::tex::ares);
      COMPARISON("HeavyMachineGun_C", &render::tex::odin);
      default:
        return &render::tex::melee;
    }
  }
}  // namespace names