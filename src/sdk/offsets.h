#ifndef SDK_OFFSETS_H
#define SDK_OFFSETS_H

#include <cstdint>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

namespace offsets {
  static uintptr_t uworld_state = 0xA2889C0;
  static uintptr_t uworld_key = uworld_state + 0x38;

  // General Offsets
  static uintptr_t fname_pool = 0xA050A00 /* 0x909F840 */;  // @image_base

  // World
  static uintptr_t persistent_level = 0x38;       // @uworld (ULevel)
  static uintptr_t owning_game_instance = 0x1A0;  // @uworld (UGameInstance)
  static uintptr_t game_state = 0x140;            // @uworld (AGameStateBase)

  // Game Instance
  static uintptr_t local_players =
      0x40;  // @game_instance (TArray<struct ULocalPlayer*>)

  // Level
  static uintptr_t actor_array = 0xA0;  // @level
  static uintptr_t actor_count =
      actor_array + 0x18;  // @level + (actor_array + 0x18) // 0xb8

  // Local Player
  static uintptr_t player_controller =
      0x38;  // @local_player (APlayerController)

  // Player Controller
  static uintptr_t acknowledged_pawn = 0x468;  // @player_controller (APawn)
  static uintptr_t player_camera =
      0x478;  // @player_controller (APlayerCameraManager)

  // Player Pawn
  static uintptr_t root_component = 0x230;  // @player_pawn (USceneComponent)
  static uintptr_t damage_handler =
      0xa10;                         // @player_pawn (UDamageableComponent)
  static uintptr_t actor_id = 0x18;  // @player_pawn (int) // UniqueID = 0x38 ?
  static uintptr_t fname_id = 0x38;  // @player_pawn (int) // FNameID = 0x18 ?
  static uintptr_t dormant = 0x100;  // @player_pawn (bool)
  static uintptr_t player_state = 0x3f8;  // @player_pawn (APlayerState)
  static uintptr_t current_mesh =
      0x430;  // @player_pawn (USkeletalMeshComponent)
  //   static uintptr_t inventory = 0x980;  // @player_pawn (UAresInventory)
  static uintptr_t inventory = 0x9a0;  // @player_pawn (UAresInventory)

  // Inventory
  static uintptr_t current_equippable = 0x248;  // @inventory (AAresEquippable)
  //   static uintptr_t current_equippable = 0x240;  // @inventory
  //   (AAresEquippable)

  // Minimap Component
  static uintptr_t local_observer = 0x530;  // @player_pawn (bool)
  static uintptr_t is_visible = 0x501;      // @player_pawn (bool)

  // Mesh
  static uintptr_t component_to_world = 0x250;  // @mesh (FTransform)
  static uintptr_t bone_array = 0x5d8;          // @mesh (FBoxSphereBounds)
  static uintptr_t bone_count =
      bone_array + 0x8;  // @mesh (int) // 5e0 // bone array cached 0x5C8
  static uintptr_t last_sub_time = 0x380;
  static uintptr_t last_render_time =
      last_sub_time + 0x4;  // @mesh + (last_sub_time + 0x4) (0x384)
  static uintptr_t attach_children = 0x110;  // @mesh (TArray<struct UMesh*>)
  static uintptr_t attach_children_count =
      attach_children + 0x8;  // @mesh (int)

  // Player State
  static uintptr_t team_component =
      0x630;  // @player_state (UBaseTeamComponent)

  // Team Component
  static uintptr_t team_id = 0xF8;  // @team_component (int)

  // Damage Handler
  static uintptr_t current_health = 0x1B0;  // @damage_handler (float)
  static uintptr_t max_life =
      current_health + 0x4;  // @damange_handler + (current_health + 0x4)

  // Root Component
  static uintptr_t relative_location = 0x164;  // @root_component (FVector)
  static uintptr_t relative_rotation = 0x170;  // @root_component (FRotator)

  // Camera Manager
  // static uintptr_t default_fov = 0x3ec;    // @camera (float)
  static uintptr_t camera_cache = 0x1fe0;  // @camera (FCameraCacheEntry)

  // FMinimalViewInfo
  static uintptr_t location = 0x0;      // @view_info + 0x0 = Location (FVector)
  static uintptr_t rotation = 0x0c;     // @view_info (FRotator)
  static uintptr_t current_fov = 0x18;  // @view_info (float)

  static uintptr_t camera_location = 0x1260;
  static uintptr_t camera_rotation = 0x126C;
  static uintptr_t camera_fov = 0x1278;

  static uintptr_t spikeExplodeOuterRadius = 0x4cc;
  static uintptr_t spikeExplodeInnerRadius = 0x4d0;
  static uintptr_t spikeTimeRemainingToExplode = 0x4d4;
  static uintptr_t spikeDefuseProgress = 0x500;

  static uintptr_t magazineAmmo =
      0xff0;  // @current_equippable (UAmmoComponent)
  static uintptr_t curAmmo =
      0x120;  // @magazineAmmo (int32), AuthResourceAmount
  static uintptr_t maxAmmo = 0x140;  // @magazineAmmo (int32)

  // FCameraCacheEntry
  static uintptr_t pov = 0x10;  // @camera_cache (FMinimalViewInfo)
  static uintptr_t veh_deref =
      0x1C62DE0;  // deref_pointer_in_game_space_fn -
                  // https://www.unknowncheats.me/forum/valorant/503616-dumping-valorant-perfect-results-easy.html
}  // namespace offsets

#pragma clang diagnostic pop

#endif /* SDK_OFFSETS_H */
