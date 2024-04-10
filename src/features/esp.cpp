#include <cocaine.h>

#include "bmain.h"
#include "config/keybinds.h"
#include "features.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "render/render.h"
#include "sdk/sdk.h"

namespace features::esp {  // features
  Vector3 angleVectors(Vector3& angles) {
    Vector3 forward;
    float sp, sy, cp, cy;
    sp = sin(DegreeToRadian(angles.x));
    sy = sin(DegreeToRadian(angles.y));
    cp = cos(DegreeToRadian(angles.x));
    cy = cos(DegreeToRadian(angles.y));
    forward.x = cp * cy;
    forward.y = cp * sy;
    forward.z = -sp;
    return forward;
  }
  void renderBox(ImDrawList* dl, Actor* actor) {
    auto headPos = actor->getBonePosition(8);  // 8 = HEAD
    auto footPos = actor->getBonePosition(0);  // 0 = FOOT

    auto head = W2S(headPos);
    auto foot = W2S(footPos);
    auto box = head - foot;
    box.x -= 5.f;
    box.y -= 5.f;
    if (isnan(box.x) || isnan(box.y)) return;

    float distance = sdk::camPos.Distance(headPos);

    if (box.y < 0) box.y *= -1;

    int bw = box.y / 2.f + 10.f;
    int dx = head.x - (bw / 2);
    int dy = head.y - 10;

    int addtlHeight = 10.f;
    auto boxWidth = bw + 1;
    auto boxHeight = box.y + addtlHeight;

    // Dormancy modified colors
    Color colWhite = Color::White;
    Color colBbox = config::c.boundingBoxC;
    Color colName = config::c.nameC;
    Color colHp = Color("#96c83ccc");
    Color colAmmo = Color("#3c78b4cc");
    auto f200a = 200;
    auto f255a = 200;
    if (actor->isDormant()) {
      auto dormantAlpha = config::c.dormantC.a();
      colWhite.setAlpha(dormantAlpha);
      colBbox.setAlpha(dormantAlpha);
      colName.setAlpha(dormantAlpha);
      colHp.setAlpha(dormantAlpha);
      colAmmo.setAlpha(dormantAlpha);
      f200a = dormantAlpha;
      f255a = dormantAlpha;
    }

    if (config::c.boundingBox) {
      render::drawBox(dl, dx, dy, boxWidth, boxHeight, colBbox, true);
    }
    auto health = actor->getHealth();

    if (config::c.showName) {
      render::drawTextCentered(dl, dx + (boxWidth / 2.f), dy - 13,
                               render::verdana, actor->getAgent(), colName,
                               12.f, false, true);
    }

    if (true) {
      auto flags = actor->espFlags;
      auto fyInc = 9.f;
      auto fx = dx + boxWidth + 4.f;
      auto fy = dy - fyInc;
      if (flags.LightArmor)
        render::drawText(dl, fx, fy += fyInc, render::sp7, "K",
                         Color(255, 255, 255, f200a), 10.f, true);
      if (flags.HeavyArmor)
        render::drawText(dl, fx, fy += fyInc, render::sp7, "HK",
                         Color(255, 255, 255, f200a), 10.f, true);
      if (flags.Zoom)
        render::drawText(dl, fx, fy += fyInc, render::sp7, "ZOOM",
                         Color(50, 150, 180, f255a), 10.f, true);
      if (flags.Blind)
        render::drawText(dl, fx, fy += fyInc, render::sp7, "BLIND",
                         Color(50, 150, 180, f255a), 10.f, true);
      if (flags.Reload)
        render::drawText(dl, fx, fy += fyInc, render::sp7, "RELOAD",
                         Color(50, 150, 180, f255a), 10.f, true);
      if (flags.Spike)
        render::drawText(dl, fx, fy += fyInc, render::sp7, "C4",
                         Color(255, 0, 0, f255a), 10.f, true);
    }

    if (config::c.healthBar) {
      auto barX = (dx - 6);
      auto barY = dy - 1.f;
      auto barWidth = 4;
      auto barHeight = box.y + addtlHeight + 2.f;

      render::drawBarVertical(
          dl, barX, barY, barWidth, barHeight, health / 100.f, colHp,
          std::to_string(static_cast<int>(health)), health < 100.f);
    }

    auto boxCenterX = dx + (boxWidth / 2.f);
    float bfY = dy + boxHeight + (config::c.showAmmo ? 8.f : 4.f);
    if (config::c.showDst) {
      static char distBuf[36];
      sprintf_s(distBuf, "%.0fM", distance / 100);
      render::drawTextCentered(dl, boxCenterX, bfY, render::sp7, distBuf,
                               colWhite, 10.f, true);
      bfY += 10.f;
    }

    if (config::c.weapIcon || config::c.weapText || config::c.showAmmo) {
      auto equip = actor->getCurrentEquippable();
      if (equip != 0) {
        auto wepId = read<int32_t>(equip + offsets::actor_id);
        uintptr_t magAmmo = read<uintptr_t>(equip + offsets::magazineAmmo);
        int32_t curAmmo = 0;
        int32_t maxAmmo = 0;

        if (magAmmo != 0) {
          rd(magAmmo + offsets::curAmmo, curAmmo);
          rd(magAmmo + offsets::maxAmmo, maxAmmo);
        }

        auto name = names::getWeaponName(wepId);
        auto tex = names::getWeaponTexture(wepId);

        if (config::c.showAmmo) {
          double ammoFloat =
              static_cast<double>(curAmmo) / static_cast<double>(maxAmmo);

          render::drawBarHorizontal(dl, dx - 1.f, dy + boxHeight + 3.f,
                                    boxWidth + 2.f, 4.f, ammoFloat, colAmmo,
                                    std::to_string(curAmmo), curAmmo < maxAmmo);
        }

        if (config::c.weapText) {
          render::drawTextCentered(dl, boxCenterX, bfY, render::sp7, name,
                                   colWhite, 10.f, true);
          bfY += 10.f;
        }
        if (config::c.weapIcon) {
          render::drawImage(dl, boxCenterX, bfY, tex, colWhite, .10f, true,
                            true);
        }
      }
    }
  }
  void renderBoneLine(ImDrawList* dl, Actor* enemy, Vector3 firstBonePos,
                      Vector3 secBonePos, const Color& col) {
    render::drawLine(dl, W2S(firstBonePos), W2S(secBonePos), col, 1.2f);
  }
  void renderSkeleton(ImDrawList* dl, Actor* enemy) {
    Vector3 head_position = enemy->getBonePosition(8);
    Vector3 neck_position;
    Vector3 chest_position = enemy->getBonePosition(6);
    Vector3 l_upper_arm_position;
    Vector3 l_fore_arm_position;
    Vector3 l_hand_position;
    Vector3 r_upper_arm_position;
    Vector3 r_fore_arm_position;
    Vector3 r_hand_position;
    Vector3 stomach_position = enemy->getBonePosition(4);
    Vector3 pelvis_position = enemy->getBonePosition(3);
    Vector3 l_thigh_position;
    Vector3 l_knee_position;
    Vector3 l_foot_position;
    Vector3 r_thigh_position;
    Vector3 r_knee_position;
    Vector3 r_foot_position;

    if (enemy->boneCount == 104) {  // MALE
      neck_position = enemy->getBonePosition(21);

      l_upper_arm_position = enemy->getBonePosition(23);
      l_fore_arm_position = enemy->getBonePosition(24);
      l_hand_position = enemy->getBonePosition(25);

      r_upper_arm_position = enemy->getBonePosition(49);
      r_fore_arm_position = enemy->getBonePosition(50);
      r_hand_position = enemy->getBonePosition(51);

      l_thigh_position = enemy->getBonePosition(77);
      l_knee_position = enemy->getBonePosition(78);
      l_foot_position = enemy->getBonePosition(80);

      r_thigh_position = enemy->getBonePosition(84);
      r_knee_position = enemy->getBonePosition(85);
      r_foot_position = enemy->getBonePosition(87);
    } else if (enemy->boneCount == 101) {  // FEMALE
      neck_position = enemy->getBonePosition(21);

      l_upper_arm_position = enemy->getBonePosition(23);
      l_fore_arm_position = enemy->getBonePosition(24);
      l_hand_position = enemy->getBonePosition(25);

      r_upper_arm_position = enemy->getBonePosition(49);
      r_fore_arm_position = enemy->getBonePosition(50);
      r_hand_position = enemy->getBonePosition(51);

      l_thigh_position = enemy->getBonePosition(75);
      l_knee_position = enemy->getBonePosition(76);
      l_foot_position = enemy->getBonePosition(78);

      r_thigh_position = enemy->getBonePosition(82);
      r_knee_position = enemy->getBonePosition(83);
      r_foot_position = enemy->getBonePosition(85);
    } else if (enemy->boneCount == 103) {  // BOT
      neck_position = enemy->getBonePosition(9);

      l_upper_arm_position = enemy->getBonePosition(33);
      l_fore_arm_position = enemy->getBonePosition(30);
      l_hand_position = enemy->getBonePosition(32);

      r_upper_arm_position = enemy->getBonePosition(58);
      r_fore_arm_position = enemy->getBonePosition(55);
      r_hand_position = enemy->getBonePosition(57);

      l_thigh_position = enemy->getBonePosition(63);
      l_knee_position = enemy->getBonePosition(65);
      l_foot_position = enemy->getBonePosition(69);

      r_thigh_position = enemy->getBonePosition(77);
      r_knee_position = enemy->getBonePosition(79);
      r_foot_position = enemy->getBonePosition(83);
    } else {
      return;
    }

    auto boneColor = config::c.skeletonC;
    if (enemy->isDormant()) boneColor = config::c.dormantC;

    renderBoneLine(dl, enemy, head_position, neck_position, boneColor);
    renderBoneLine(dl, enemy, neck_position, chest_position, boneColor);
    renderBoneLine(dl, enemy, neck_position, l_upper_arm_position, boneColor);
    renderBoneLine(dl, enemy, l_upper_arm_position, l_fore_arm_position,
                   boneColor);
    renderBoneLine(dl, enemy, l_fore_arm_position, l_hand_position, boneColor);
    renderBoneLine(dl, enemy, neck_position, r_upper_arm_position, boneColor);
    renderBoneLine(dl, enemy, r_upper_arm_position, r_fore_arm_position,
                   boneColor);
    renderBoneLine(dl, enemy, r_fore_arm_position, r_hand_position, boneColor);
    renderBoneLine(dl, enemy, chest_position, stomach_position, boneColor);
    renderBoneLine(dl, enemy, stomach_position, pelvis_position, boneColor);
    renderBoneLine(dl, enemy, pelvis_position, l_thigh_position, boneColor);
    renderBoneLine(dl, enemy, l_thigh_position, l_knee_position, boneColor);
    renderBoneLine(dl, enemy, l_knee_position, l_foot_position, boneColor);
    renderBoneLine(dl, enemy, pelvis_position, r_thigh_position, boneColor);
    renderBoneLine(dl, enemy, r_thigh_position, r_knee_position, boneColor);
    renderBoneLine(dl, enemy, r_knee_position, r_foot_position, boneColor);
  }

  void renderAimline(ImDrawList* dl, Actor* actor) {
    auto rot = actor->getRotation();

    Vector3 headPos = actor->getBonePosition(8);  // 8 = HEAD
    Vector3 vRot(rot.Pitch, rot.Yaw, rot.Roll);

    Vector3 fwd = angleVectors(vRot);
    fwd.x *= 125;
    fwd.y *= 125;
    fwd.z *= 125;
    Vector3 endPos = headPos + fwd;

    auto lineColor = config::c.viewLinesC;
    if (actor->isDormant()) lineColor = config::c.dormantC;

    render::drawLine(dl, W2S(headPos), W2S(endPos), lineColor);
  }
  void renderHeadDot(ImDrawList* dl, Actor* actor) {
    auto headPos = actor->getBonePosition(8);
    auto rootPos = actor->getPosition();
    if (headPos.z <= rootPos.z) return;
    auto head = W2S(headPos);

    auto col = actor->isDormant() ? config::c.dormantC : config::c.headDotC;
    render::drawCircle(dl, head, 5.f, col, false, 2.f);
  }
  void renderFootstep(ImDrawList* dl, Actor* actor) {
    if (!config::c.footsteps) return;
    if (!config::c.teammates && !actor->isEnemy()) return;
    if (actor->uniqueId != 655617) return;
    auto wp0 = actor->getPosition();
    if (isnan(wp0.x) || isnan(wp0.y)) return;

    auto diff = ImGui::GetFrameCount() - actor->startFrame;
    if (diff > (3 * 165.f)) return;  // bad

    auto radius = (ImGui::GetFrameCount() - actor->startFrame) % 60 * 1.5f;
    auto footstepColor = config::c.footstepsC;
    render::drawWorldCircle(dl, wp0, 180.f, radius, footstepColor);
  }
  void renderSpike(ImDrawList* dl, Actor* actor) {
    if (!config::c.spike) return;
    if (actor->uniqueId != 17694977) return;
    auto timer =
        read<float>(actor->actorPtr + offsets::spikeTimeRemainingToExplode);
    auto defuseProgress =
        read<float>(actor->actorPtr + offsets::spikeDefuseProgress);
    auto outerRadius =
        read<float>(actor->actorPtr + offsets::spikeExplodeOuterRadius);
    if (defuseProgress > 8) return;
    auto wPos = actor->getPosition();
    auto sPos = actor->getScreenPosition();
    if (isnan(wPos.x) || isnan(wPos.y)) return;
    if (isnan(sPos.x) || isnan(sPos.y)) return;

    auto text = names::format("Spike [%.2f]", timer);
    auto meas =
        render::verdana->CalcTextSizeA(12.f, FLT_MAX, FLT_MAX, text.c_str());
    render::drawTextCentered(dl, sPos, render::sp7, text, config::c.spikeC,
                             10.f, true, false);

    auto dx = sPos.x - (meas.x / 2);
    auto dy = sPos.y + 20.f;
    auto barW = meas.x;
    auto barH = 4;

    render::drawBarHorizontal(dl, dx, dy, barW, barH, timer / 45.f,
                              config::c.spikeC);
    render::drawBarHorizontal(dl, dx, dy + 6.f, barW, barH,
                              defuseProgress / 7.f,
                              Color(0xff, 0x95, 0x80, 0xff));

    render::drawWorldCircle(dl, actor->getPosition(), 180.f, outerRadius,
                            Color(0xff, 0x80, 0x95, 0x90));
  }
}  // namespace features::esp

#define PTRCHECK(ptr, breakOut)                                  \
  if (ptr == 0) {                                                \
    DbgLog("Ptr " #ptr " is nullptr, ret early\n");              \
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); \
    breakOut;                                                    \
  }

namespace features::esp {
  std::mutex mutPlayers;
  std::mutex mutActors;
  ImDrawList* playerDrawList;
  ImDrawList* actorDrawList;
  ImDrawData drawData;

  bool drawDataSetup = false;

  void renderPlayer(ImDrawList* dl, Actor* player) {
    if (sdk::lpa->actorPtr == player->actorPtr) return;
    if (!config::c.teammates && (!player->isEnemy() && !player->isBot()))
      return;
    if (!player->isAlive()) return;

    renderBox(dl, player);
    if (config::c.skeleton) renderSkeleton(dl, player);
    if (config::c.viewLines) renderAimline(dl, player);
    if (config::c.headDot) renderHeadDot(dl, player);
  }

  void renderActor(ImDrawList* dl, Actor* actor) {
    switch (actor->actorType) {
      case ActorType::Footstep:
        renderFootstep(dl, actor);
        break;
      case ActorType::Spike:
        renderSpike(dl, actor);
        break;
      default:
        break;
    }
  }

  void renderPlayers() {
    if (!render::initialized) return;
    if (!playerDrawList) {
      playerDrawList = new ImDrawList(ImGui::GetDrawListSharedData());
    }
    if (!sdk::World || !sdk::LocalPlayerPawn || !sdk::GameState) return;

    auto gameBase = read<AGameStateBase>(sdk::GameState);

    playerDrawList->_ResetForNewFrame();

    playerDrawList->PushTextureID(ImGui::GetIO().Fonts->TexID);
    playerDrawList->PushClipRectFullScreen();

    sdk::iteratePlayers(&gameBase.PlayerArray, [](Actor* player) {
      features::esp::renderPlayer(playerDrawList, player);
    });
  }

  void render() {
    if (!keybinds::esp) return;
    if (!drawDataSetup) {
      drawData.Valid = true;
      drawData.CmdLists = new ImDrawList*[2];
      drawData.CmdListsCount = 2;
      drawData.DisplayPos = {0.f, 0.f};
      drawData.DisplaySize = {static_cast<float>(sdk::width),
                              static_cast<float>(sdk::height)};
      drawData.FramebufferScale = {1.f, 1.f};
      drawDataSetup = true;
    }

    auto cameraCache =
        read<FCameraCacheEntry>(sdk::localCamera + offsets::camera_cache);
    sdk::camPos = cameraCache.POV.location;
    sdk::camRot = cameraCache.POV.rotation;
    sdk::camFov = cameraCache.POV.fov;

    // auto vp = reinterpret_cast<ImGuiViewportP*>(ImGui::GetMainViewport());

    drawData.TotalVtxCount = 0;
    drawData.TotalIdxCount = 0;

    renderPlayers();
    drawData.CmdLists[0] = playerDrawList;
    drawData.TotalVtxCount += drawData.CmdLists[0]->VtxBuffer.size();
    drawData.TotalIdxCount += drawData.CmdLists[0]->IdxBuffer.size();

    mutActors.lock();
    drawData.CmdLists[1] = actorDrawList->CloneOutput();
    drawData.TotalVtxCount += drawData.CmdLists[1]->VtxBuffer.size();
    drawData.TotalIdxCount += drawData.CmdLists[1]->IdxBuffer.size();
    mutActors.unlock();
    ImGui_ImplOpenGL3_RenderDrawData(&drawData);
  }
}  // namespace features::esp