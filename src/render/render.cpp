#include "render.h"

#include <gl.h>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ios>
#include <sstream>

#include "bmain.h"
#include "cocaine.h"
#include "config/keybinds.h"
#include "generated/font_sp7.h"
#include "hid/hid.h"
#include "rimg.h"
#include "sdk/sdk.h"

namespace render {  // primitives
  ImFont* verdana;
  ImFont* sp7;
  ImFont* sp7Out;
  GLFWwindow* window;
  std::atomic_bool initialized = false;
  std::atomic_bool mousePassthrough = false;
  Color outlineColor = Color::Black;
  // explicit coordinates:
  void drawLine(ImDrawList* dl, float x1, float y1, float x2, float y2,
                const Color& col, float thickness) {
    dl->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), col.getRawColor(), thickness);
  }
  void drawBox(ImDrawList* dl, float x, float y, float w, float h,
               const Color& col, bool outline, bool inl) {
    if (outline) {
      if (inl) {
        dl->AddRect(ImVec2(x + 1, y + 1), ImVec2(x + w - 1, y + h - 1),
                    outlineColor.getRawColor(), 0.f, ImDrawFlags_None, 1.f);
      }
      dl->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1),
                  outlineColor.getRawColor(), 0.f, ImDrawFlags_None, 1.f);
    }

    dl->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), col.getRawColor(), 0.f,
                ImDrawFlags_None, 1.f);
  }
  void drawBoxFilled(ImDrawList* dl, float x, float y, float w, float h,
                     const Color& col) {
    dl->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), col.getRawColor());
  }

  void drawText(ImDrawList* dl, float x, float y, ImFont* font, const char* str,
                const Color& col, float size, bool outline, bool shadow) {
    if (font == sp7) {
      size = font->FontSize;
    }
    if (shadow) {
      dl->AddText(font, size, ImVec2(x + 1, y + 1), Color::Black.getRawColor(),
                  str);
    }

    if (outline && font == sp7) {
      dl->AddText(sp7Out, size, ImVec2(x, y), Color::Black.getRawColor(), str);
    }

    dl->AddText(font, size, ImVec2(x, y), col.getRawColor(), str);
  }
  void drawText(ImDrawList* dl, float x, float y, ImFont* font, std::string str,
                const Color& col, float size, bool outline, bool shadow) {
    drawText(dl, x, y, font, str.c_str(), col, size, outline, shadow);
  }

  void drawTextCentered(ImDrawList* dl, float x, float y, ImFont* font,
                        std::string str, const Color& col, float size,
                        bool outline, bool shadow) {
    auto text = str.c_str();
    auto tm = font->CalcTextSizeA(size, FLT_MAX, FLT_MAX, text);

    drawText(dl, x - (tm.x / 2), y, font, text, col, size, outline, shadow);
  }

  void drawImage(ImDrawList* dl, float x, float y, OurTex* tex,
                 const Color& col, float scale, bool centered, bool shadow) {
    auto scaledW = tex->width * scale;
    auto scaledH = tex->height * scale;

    auto startX = x;
    if (centered) startX = startX - (scaledW / 2);

    if (shadow) {
      dl->AddImage((void*)tex->tex, ImVec2(startX + 1.f, y + 1.f),
                   ImVec2(startX + scaledW + 1.f, y + scaledH + 1.f),
                   ImVec2(0, 0), ImVec2(1, 1), outlineColor.getRawColor());
    }

    dl->AddImage((void*)tex->tex, ImVec2(startX, y),
                 ImVec2(startX + scaledW, y + scaledH), ImVec2(0, 0),
                 ImVec2(1, 1), col.getRawColor());
  }

  void drawCircle(ImDrawList* dl, float x, float y, float radius,
                  const Color& col, bool filled, float thickness) {
    if (filled) {
      dl->AddCircleFilled({x, y}, radius, col.getRawColor());

    } else {
      dl->AddCircle({x, y}, radius, col.getRawColor(), 0);
    }
  }

  void drawWorldCircle(ImDrawList* dl, Vector3 pos, float points, float radius,
                       const Color& col) {
    float step = (float)M_PI * 2.0f / points;
    for (float a = 0; a < (M_PI * 2.0f); a += step) {
      Vector3 start(radius * cosf(a) + pos.x, radius * sinf(a) + pos.y, pos.z);
      Vector3 end(radius * cosf(a + step) + pos.x,
                  radius * sinf(a + step) + pos.y, pos.z);

      Vector2 start2d, end2d;
      start2d = W2S(start);
      end2d = W2S(end);
      drawLine(dl, W2S(start), W2S(end), col);
    }
  }

  void drawWorldCircle(ImDrawList* dl, float x, float y, float z, float points,
                       float radius, const Color& col) {
    drawWorldCircle(dl, {x, y, z}, points, radius, col);
  }

  void drawBarHorizontal(ImDrawList* dl, float x, float y, float w, float h,
                         float val, const Color& col, std::string text,
                         bool drawText) {
    drawBox(dl, x, y, w, h, outlineColor);

    if (val >= 0.02f) {
      auto tw = (w - 1.f) * val;
      drawBoxFilled(dl, x + 1, y + 1, tw - 1.f, h - 2.f, col);

      if (drawText) {
        drawTextCentered(dl, x + tw, y + (h / 2.f), sp7, text, Color::White,
                         10.f, true);
      }
    }
  }
  void drawBarVertical(ImDrawList* dl, float x, float y, float w, float h,
                       float val, const Color& col, std::string text,
                       bool drawText) {
    drawBox(dl, x, y, w, h, outlineColor);

    if (val >= 0.2f) {
      auto th = (h - 1.f) * val;
      drawBoxFilled(dl, x + 1, y + (h - th), w - 2.f, th - 1.f, col);

      if (drawText) {
        drawTextCentered(dl, x + (w / 2.f), y + (h - th), sp7, text,
                         Color::White, 10.f, true);
      }
    }
  }

  // Vector2:
  void drawLine(ImDrawList* dl, Vector2 v1, Vector2 v2, const Color& col,
                float thickness) {
    drawLine(dl, v1.x, v1.y, v2.x, v2.y, col, thickness);
  }
  void drawBox(ImDrawList* dl, Vector2 pos, Vector2 dim, const Color& col,
               bool outline, bool inl) {
    drawBox(dl, pos.x, pos.y, dim.x, dim.y, col, outline, inl);
  }
  void drawBoxFilled(ImDrawList* dl, Vector2 pos, Vector2 dim,
                     const Color& col) {
    drawBoxFilled(dl, pos.x, pos.y, dim.x, dim.y, col);
  }
  void drawText(ImDrawList* dl, Vector2 pos, ImFont* font, const char* str,
                const Color& col, float size, bool outline, bool shadow) {
    drawText(dl, pos.x, pos.y, font, str, col, size, outline, shadow);
  }
  void drawText(ImDrawList* dl, Vector2 pos, ImFont* font, std::string str,
                const Color& col, float size, bool outline, bool shadow) {
    drawText(dl, pos.x, pos.y, font, str.c_str(), col, size, outline, shadow);
  }
  void drawTextCentered(ImDrawList* dl, Vector2 pos, ImFont* font,
                        std::string str, const Color& col, float size,
                        bool outline, bool shadow) {
    auto text = str.c_str();
    auto tm = font->CalcTextSizeA(size, FLT_MAX, FLT_MAX, text);

    drawText(dl, pos.x - (tm.x / 2), pos.y, font, text, col, size, outline,
             shadow);
  }
  void drawImage(ImDrawList* dl, Vector2 pos, OurTex* tex, const Color& col,
                 float scale, bool centered, bool shadow) {
    drawImage(dl, pos.x, pos.y, tex, col, scale, centered, shadow);
  }
  void drawCircle(ImDrawList* dl, Vector2 pos, float radius, const Color& col,
                  bool filled, float thickness) {
    drawCircle(dl, pos.x, pos.y, radius, col, filled, thickness);
  }

  void setup(GLFWwindow* window) {
    render::window = window;
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 130");

    verdana = io.FontDefault = io.Fonts->AddFontFromFileTTF(
        "C:\\windows\\fonts\\verdana.ttf", 12.f, nullptr);

    ImFontConfig cfgIn;
    cfgIn.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome |
                             ImGuiFreeTypeBuilderFlags_MonoHinting;
    sp7 = io.Fonts->AddFontFromMemoryTTF((void*)FONT_SP7, FONT_SP7_size, 10.f,
                                         &cfgIn);

    ImFontConfig cfgOut;
    cfgOut.FontBuilderFlags =
        cfgIn.FontBuilderFlags | ImGuiFreeTypeBuilderFlags_Stroke;
    sp7Out = io.Fonts->AddFontFromMemoryTTF((void*)FONT_SP7, FONT_SP7_size,
                                            10.f, &cfgOut);
    outlineColor.setColor(0, 0, 0, 190);

    render::loadTextures();
    initialized = true;
  }

  void shutdown() { ImGui::DestroyContext(); }

}  // namespace render
