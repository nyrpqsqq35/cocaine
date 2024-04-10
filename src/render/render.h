#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

#include <gl.h>
#include <imgui.h>

#include "config/Color.h"
#include "rimg.h"
#include "sdk/vector.h"

namespace render {
  extern ImFont* verdana;
  extern ImFont* sp7;
  extern ImFont* sp7Out;
  extern GLFWwindow* window;
  extern std::atomic_bool initialized;
  extern std::atomic_bool mousePassthrough;
  void drawLine(ImDrawList* dl, float x1, float y1, float x2, float y2,
                const Color& col, float thickness = 1.f);
  void drawBox(ImDrawList* dl, float x, float y, float w, float h,
               const Color& col, bool outline = false, bool inl = true);
  void drawBoxFilled(ImDrawList* dl, float x, float y, float w, float h,
                     const Color& col);
  void drawText(ImDrawList* dl, float x, float y, ImFont* font, const char* str,
                const Color& col, float size = 12.f, bool outline = false,
                bool shadow = false);
  void drawText(ImDrawList* dl, float x, float y, ImFont* font, std::string str,
                const Color& col, float size = 12.f, bool outline = false,
                bool shadow = false);
  void drawTextCentered(ImDrawList* dl, float x, float y, ImFont* font,
                        std::string str, const Color& col, float size = 12.f,
                        bool outline = false, bool shadow = false);
  void drawImage(ImDrawList* dl, float x, float y, OurTex* tex,
                 const Color& col, float scale = 1.f, bool centered = false,
                 bool shadow = false);
  void drawCircle(ImDrawList* dl, float x, float y, float radius,
                  const Color& col, bool filled = false, float thickness = 1.f);
  void drawWorldCircle(ImDrawList* dl, Vector3 pos, float points, float radius,
                       const Color& col);
  void drawWorldCircle(ImDrawList* dl, float x, float y, float z, float points,
                       float radius, const Color& col);
  void drawBarHorizontal(ImDrawList* dl, float x, float y, float w, float h,
                         float val, const Color& col, std::string text = "",
                         bool drawText = false);
  void drawBarVertical(ImDrawList* dl, float x, float y, float w, float h,
                       float val, const Color& col, std::string text = "",
                       bool drawText = false);

  void drawLine(ImDrawList* dl, Vector2 v1, Vector2 v2, const Color& col,
                float thickness = 1.f);
  void drawBox(ImDrawList* dl, Vector2 pos, Vector2 dim, const Color& col,
               bool outline = false, bool inl = true);
  void drawBoxFilled(ImDrawList* dl, Vector2 pos, Vector2 dim,
                     const Color& col);
  void drawText(ImDrawList* dl, Vector2 pos, ImFont* font, const char* str,
                const Color& col, float size = 12.f, bool outline = false,
                bool shadow = false);
  void drawText(ImDrawList* dl, Vector2 pos, ImFont* font, std::string str,
                const Color& col, float size = 12.f, bool outline = false,
                bool shadow = false);
  void drawTextCentered(ImDrawList* dl, Vector2 pos, ImFont* font,
                        std::string str, const Color& col, float size = 12.f,
                        bool outline = false, bool shadow = false);
  void drawImage(ImDrawList* dl, Vector2 pos, OurTex* tex, const Color& col,
                 float scale = 1.f, bool centered = false, bool shadow = false);
  void drawCircle(ImDrawList* dl, Vector2 pos, float radius, const Color& col,
                  bool filled = false, float thickness = 1.f);
  void setup(GLFWwindow* window);
  void shutdown();
}  // namespace render

#endif /* RENDER_RENDER_H */
