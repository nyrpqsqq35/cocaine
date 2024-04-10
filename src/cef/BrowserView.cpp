// This code is a modification of the original projects that can be found at
// https://github.com/if1live/cef-gl-example
// https://github.com/andmcgregor/cefgui

#include "BrowserView.hpp"

#include "GLCore.hpp"
#include "config/config.h"
#include "render/render.h"
#include "v8handler.h"

namespace {
#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39
#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A
#define VK_UNDEF 0x07
  uint32_t glfwKeyCodeToVK(uint32_t glfwKey) {
    switch (glfwKey) {
      case GLFW_KEY_SPACE:
        return VK_SPACE;
      case GLFW_KEY_APOSTROPHE:
        return VK_OEM_7;
      case GLFW_KEY_COMMA:
        return VK_OEM_COMMA;
      case GLFW_KEY_MINUS:
        return VK_OEM_MINUS;
      case GLFW_KEY_PERIOD:
        return VK_OEM_PERIOD;
      case GLFW_KEY_SLASH:
        return VK_OEM_2;

      case GLFW_KEY_0:
        return VK_0;
      case GLFW_KEY_1:
        return VK_1;
      case GLFW_KEY_2:
        return VK_2;
      case GLFW_KEY_3:
        return VK_3;
      case GLFW_KEY_4:
        return VK_4;
      case GLFW_KEY_5:
        return VK_5;
      case GLFW_KEY_6:
        return VK_6;
      case GLFW_KEY_7:
        return VK_7;
      case GLFW_KEY_8:
        return VK_8;
      case GLFW_KEY_9:
        return VK_9;

      case GLFW_KEY_SEMICOLON:
        return VK_OEM_1;
      case GLFW_KEY_EQUAL:
        return VK_OEM_PLUS;  // +=

      case GLFW_KEY_A:
        return VK_A;
      case GLFW_KEY_B:
        return VK_B;
      case GLFW_KEY_C:
        return VK_C;
      case GLFW_KEY_D:
        return VK_D;
      case GLFW_KEY_E:
        return VK_E;
      case GLFW_KEY_F:
        return VK_F;
      case GLFW_KEY_G:
        return VK_G;
      case GLFW_KEY_H:
        return VK_H;
      case GLFW_KEY_I:
        return VK_I;
      case GLFW_KEY_J:
        return VK_J;
      case GLFW_KEY_K:
        return VK_K;
      case GLFW_KEY_L:
        return VK_L;
      case GLFW_KEY_M:
        return VK_M;
      case GLFW_KEY_N:
        return VK_N;
      case GLFW_KEY_O:
        return VK_O;
      case GLFW_KEY_P:
        return VK_P;
      case GLFW_KEY_Q:
        return VK_Q;
      case GLFW_KEY_R:
        return VK_R;
      case GLFW_KEY_S:
        return VK_S;
      case GLFW_KEY_T:
        return VK_T;
      case GLFW_KEY_U:
        return VK_U;
      case GLFW_KEY_V:
        return VK_V;
      case GLFW_KEY_W:
        return VK_W;
      case GLFW_KEY_X:
        return VK_X;
      case GLFW_KEY_Y:
        return VK_Y;
      case GLFW_KEY_Z:
        return VK_Z;

      case GLFW_KEY_LEFT_BRACKET:
        return VK_OEM_4;
      case GLFW_KEY_BACKSLASH:
        return VK_OEM_5;
      case GLFW_KEY_RIGHT_BRACKET:
        return VK_OEM_6;
      case GLFW_KEY_GRAVE_ACCENT:
        return VK_OEM_3;
      case GLFW_KEY_WORLD_1:
        return VK_UNDEF;
      case GLFW_KEY_WORLD_2:
        return VK_UNDEF;

      case GLFW_KEY_ESCAPE:
        return VK_ESCAPE;
      case GLFW_KEY_ENTER:
        return VK_RETURN;
      case GLFW_KEY_TAB:
        return VK_TAB;
      case GLFW_KEY_BACKSPACE:
        return VK_BACK;
      case GLFW_KEY_INSERT:
        return VK_INSERT;
      case GLFW_KEY_DELETE:
        return VK_DELETE;
      case GLFW_KEY_RIGHT:
        return VK_RIGHT;
      case GLFW_KEY_LEFT:
        return VK_LEFT;
      case GLFW_KEY_DOWN:
        return VK_DOWN;
      case GLFW_KEY_UP:
        return VK_UP;
      case GLFW_KEY_PAGE_UP:
        return VK_PRIOR;
      case GLFW_KEY_PAGE_DOWN:
        return VK_NEXT;
      case GLFW_KEY_HOME:
        return VK_HOME;
      case GLFW_KEY_END:
        return VK_END;
      case GLFW_KEY_CAPS_LOCK:
        return VK_CAPITAL;
      case GLFW_KEY_SCROLL_LOCK:
        return VK_SCROLL;
      case GLFW_KEY_NUM_LOCK:
        return VK_NUMLOCK;
      case GLFW_KEY_PRINT_SCREEN:
        return VK_SNAPSHOT;
      case GLFW_KEY_PAUSE:
        return VK_PAUSE;
      case GLFW_KEY_F1:
        return VK_F1;
      case GLFW_KEY_F2:
        return VK_F2;
      case GLFW_KEY_F3:
        return VK_F3;
      case GLFW_KEY_F4:
        return VK_F4;
      case GLFW_KEY_F5:
        return VK_F5;
      case GLFW_KEY_F6:
        return VK_F6;
      case GLFW_KEY_F7:
        return VK_F7;
      case GLFW_KEY_F8:
        return VK_F8;
      case GLFW_KEY_F9:
        return VK_F9;
      case GLFW_KEY_F10:
        return VK_F10;
      case GLFW_KEY_F11:
        return VK_F11;
      case GLFW_KEY_F12:
        return VK_F12;
      case GLFW_KEY_F13:
        return VK_F13;
      case GLFW_KEY_F14:
        return VK_F14;
      case GLFW_KEY_F15:
        return VK_F15;
      case GLFW_KEY_F16:
        return VK_F16;
      case GLFW_KEY_F17:
        return VK_F17;
      case GLFW_KEY_F18:
        return VK_F18;
      case GLFW_KEY_F19:
        return VK_F19;
      case GLFW_KEY_F20:
        return VK_F20;
      case GLFW_KEY_F21:
        return VK_F21;
      case GLFW_KEY_F22:
        return VK_F22;
      case GLFW_KEY_F23:
        return VK_F23;
      case GLFW_KEY_F24:
        return VK_F24;
      case GLFW_KEY_F25:
        return VK_UNDEF;

      case GLFW_KEY_KP_0:
        return VK_NUMPAD0;
      case GLFW_KEY_KP_1:
        return VK_NUMPAD1;
      case GLFW_KEY_KP_2:
        return VK_NUMPAD2;
      case GLFW_KEY_KP_3:
        return VK_NUMPAD3;
      case GLFW_KEY_KP_4:
        return VK_NUMPAD4;
      case GLFW_KEY_KP_5:
        return VK_NUMPAD5;
      case GLFW_KEY_KP_6:
        return VK_NUMPAD6;
      case GLFW_KEY_KP_7:
        return VK_NUMPAD7;
      case GLFW_KEY_KP_8:
        return VK_NUMPAD8;
      case GLFW_KEY_KP_9:
        return VK_NUMPAD9;
      case GLFW_KEY_KP_DECIMAL:
        return VK_DECIMAL;
      case GLFW_KEY_KP_DIVIDE:
        return VK_DIVIDE;
      case GLFW_KEY_KP_MULTIPLY:
        return VK_MULTIPLY;
      case GLFW_KEY_KP_SUBTRACT:
        return VK_SUBTRACT;
      case GLFW_KEY_KP_ADD:
        return VK_ADD;
      case GLFW_KEY_KP_ENTER:
        return VK_RETURN;
      case GLFW_KEY_KP_EQUAL:
        return VK_OEM_PLUS;

      case GLFW_KEY_LEFT_SHIFT:
        return VK_LSHIFT;
      case GLFW_KEY_LEFT_CONTROL:
        return VK_LCONTROL;
      case GLFW_KEY_LEFT_ALT:
        return VK_LMENU;
      case GLFW_KEY_LEFT_SUPER:
        return VK_LWIN;
      case GLFW_KEY_RIGHT_SHIFT:
        return VK_RSHIFT;
      case GLFW_KEY_RIGHT_CONTROL:
        return VK_RCONTROL;
      case GLFW_KEY_RIGHT_ALT:
        return VK_RMENU;
      case GLFW_KEY_RIGHT_SUPER:
        return VK_RWIN;
      case GLFW_KEY_MENU:
        return VK_MENU;
      default:
        return VK_UNDEF;
    }
  }

  CefBrowserHost::MouseButtonType getMouseButton(int btn) {
    switch (btn) {
      case GLFW_MOUSE_BUTTON_LEFT:
        return MBT_LEFT;
      case GLFW_MOUSE_BUTTON_MIDDLE:
        return MBT_MIDDLE;
      case GLFW_MOUSE_BUTTON_RIGHT:
        return MBT_RIGHT;
      default:
        return MBT_LEFT;
    }
  }
}  // namespace

bool BrowserView::BrowserClient::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
  auto msgName = message->GetName();
  if (msgName == kSetPointerLock) {
    auto pointerLock = message->GetArgumentList()->GetBool(0);
    render::mousePassthrough = !pointerLock;
    glfwSetWindowAttrib(render::window, GLFW_MOUSE_PASSTHROUGH,
                        render::mousePassthrough);
    return true;
  } else if (msgName == kSetConfig) {
    auto configStr = message->GetArgumentList()->GetString(0);
    config::setConfig(configStr);
    return true;
  } else if (msgName == kSetGlobalConfig) {
    auto configStr = message->GetArgumentList()->GetString(0);
    config::setGlobalConfig(configStr);
    return true;
  }
  return false;
}

BrowserView::RenderHandler::RenderHandler(const glm::vec4& viewport)
    : m_viewport(viewport) {}

BrowserView::RenderHandler::~RenderHandler() {
  // Free GPU memory
  // GLCore::deleteProgram(m_prog);
  // Crash 0x70
  // glDeleteBuffers(1, &m_vbo);
  // glDeleteVertexArrays(1, &m_vao);
}

bool BrowserView::RenderHandler::init() {
  // Dummy texture data
  const unsigned char data[] = {
      255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255, 255,
  };

  // Compile vertex and fragment shaders
  m_prog = GLCore::createShaderProgram("shaders/tex.vert", "shaders/tex.frag");
  if (m_prog == 0) {
    std::cerr << "shader compile failed" << std::endl;
    return false;
  }

  // Get locations of shader variables (attributes and uniforms)
  m_pos_loc = GLCHECK(glGetAttribLocation(m_prog, "position"));
  m_tex_loc = GLCHECK(glGetUniformLocation(m_prog, "tex"));
  m_mvp_loc = GLCHECK(glGetUniformLocation(m_prog, "mvp"))

      // Square vertices (texture positions are computed directly inside the
      // shader)
      float coords[] = {-1.0, -1.0, -1.0, 1.0, 1.0, -1.0,
                        1.0,  -1.0, -1.0, 1.0, 1.0, 1.0};

  // See https://learnopengl.com/Getting-started/Textures
  GLCHECK(glGenVertexArrays(1, &m_vao));
  GLCHECK(glBindVertexArray(m_vao));
  GLCHECK(glGenBuffers(1, &m_vbo));
  GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
  GLCHECK(
      glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW));
  GLCHECK(glEnableVertexAttribArray(m_pos_loc));
  GLCHECK(glVertexAttribPointer(m_pos_loc, 2, GL_FLOAT, GL_FALSE, 0, 0));

  GLCHECK(glGenTextures(1, &m_tex));
  GLCHECK(glBindTexture(GL_TEXTURE_2D, m_tex));
  GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, data));
  GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

  GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GLCHECK(glBindVertexArray(0));

  return true;
}

void BrowserView::RenderHandler::draw(const glm::vec4& viewport, bool fixed) {
  // Where to paint on the OpenGL window
  GLCHECK(glViewport(viewport[0], viewport[1], GLsizei(viewport[2] * m_width),
                     GLsizei(viewport[3] * m_height)));

  // Apply a rotation
  glm::mat4 trans = glm::mat4(1.0f);  // Identity matrix
  if (!fixed) {
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime() / 5.0f,
                        glm::vec3(0.0f, 0.0f, 1.0f));
  }

  // See https://learnopengl.com/Getting-started/Textures
  GLCHECK(glUseProgram(m_prog));
  GLCHECK(glBindVertexArray(m_vao));

  GLCHECK(glUniformMatrix4fv(m_mvp_loc, 1, GL_FALSE, glm::value_ptr(trans)));
  GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
  GLCHECK(glActiveTexture(GL_TEXTURE0));
  GLCHECK(glBindTexture(GL_TEXTURE_2D, m_tex));
  GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
  GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
  GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

  GLCHECK(glBindVertexArray(0));
  GLCHECK(glUseProgram(0));
}

void BrowserView::RenderHandler::reshape(int w, int h) {
  m_width = w;
  m_height = h;
}

bool BrowserView::viewport(float x, float y, float w, float h) {
  if (!(x >= 0.0f) && (x < 1.0f)) return false;

  if (!(x >= 0.0f) && (y < 1.0f)) return false;

  if (!(w > 0.0f) && (w <= 1.0f)) return false;

  if (!(h > 0.0f) && (h <= 1.0f)) return false;

  if (x + w > 1.0f) return false;

  if (y + h > 1.0f) return false;

  m_viewport[0] = x;
  m_viewport[1] = y;
  m_viewport[2] = w;
  m_viewport[3] = h;

  return true;
}

void BrowserView::RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser,
                                             CefRect& rect) {
  rect = CefRect(m_viewport[0], m_viewport[1], m_viewport[2] * m_width,
                 m_viewport[3] * m_height);
}

void BrowserView::RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                                         PaintElementType type,
                                         const RectList& dirtyRects,
                                         const void* buffer, int width,
                                         int height) {
  // std::cout << "BrowserView::RenderHandler::OnPaint" << std::endl;
  GLCHECK(glActiveTexture(GL_TEXTURE0));
  GLCHECK(glBindTexture(GL_TEXTURE_2D, m_tex));
  GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT,
                       GL_UNSIGNED_BYTE, (unsigned char*)buffer));
  GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

BrowserView::BrowserView(const std::string& url)
    : m_mouse_x(0), m_mouse_y(0), m_viewport(0.0f, 0.0f, 1.0f, 1.0f) {
  CefWindowInfo window_info;
  window_info.SetAsWindowless(0);

  m_render_handler = new RenderHandler(m_viewport);
  m_initialized = m_render_handler->init();
  m_render_handler->reshape(128, 128);  // initial size

  CefBrowserSettings browserSettings;
  browserSettings.windowless_frame_rate = 60;  // 30 is default

  DEVMODEA mode{};
  if (EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &mode)) {
    browserSettings.windowless_frame_rate = mode.dmDisplayFrequency;
    printf("Our refresh rate: %lu\n", mode.dmDisplayFrequency);
  }
  m_client = new BrowserClient(m_render_handler);
  m_browser = CefBrowserHost::CreateBrowserSync(
      window_info, m_client.get(), url, browserSettings, nullptr, nullptr);
}

BrowserView::~BrowserView() {
  CefDoMessageLoopWork();
  m_browser->GetHost()->CloseBrowser(true);

  m_browser = nullptr;
  m_client = nullptr;
}

void BrowserView::load(const std::string& url) {
  assert(m_initialized);
  m_browser->GetMainFrame()->LoadURL(url);
}

void BrowserView::draw() {
  CefDoMessageLoopWork();
  m_render_handler->draw(m_viewport, m_fixed);
}

void BrowserView::reshape(int w, int h) {
  m_render_handler->reshape(w, h);
  GLCHECK(glViewport(m_viewport[0], m_viewport[1], GLsizei(m_viewport[2] * w),
                     GLsizei(m_viewport[3] * h)));
  m_browser->GetHost()->WasResized();
}

void BrowserView::mouseMove(int x, int y, GLModifiers_ mod) {
  m_mouse_x = x;
  m_mouse_y = y;

  CefMouseEvent evt;
  evt.x = x;
  evt.y = y;

  if (mod.shift) evt.modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (mod.ctrl) evt.modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (mod.alt) evt.modifiers |= EVENTFLAG_ALT_DOWN;
  if (mod.leftMouse) evt.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
  if (mod.middleMouse) evt.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
  if (mod.rightMouse) evt.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
  bool mouse_leave = false;  // TODO
  m_browser->GetHost()->SendMouseMoveEvent(evt, mouse_leave);
}

void BrowserView::mouseClick(int btn, int action, int mods) {
  CefMouseEvent evt;
  evt.x = m_mouse_x;
  evt.y = m_mouse_y;

  if (btn == GLFW_MOUSE_BUTTON_4 || btn == GLFW_MOUSE_BUTTON_5) {
    auto msg = CefProcessMessage::Create(kCustomMouseEvent);
    msg->GetArgumentList()->SetBool(0, action == GLFW_PRESS);
    msg->GetArgumentList()->SetInt(1, m_mouse_x);
    msg->GetArgumentList()->SetInt(2, m_mouse_y);
    msg->GetArgumentList()->SetInt(3, btn);
    m_browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, msg);
  } else {
    auto cefBtn = getMouseButton(btn);
    int click_count = 1;  // TODO
    m_browser->GetHost()->SendMouseClickEvent(
        evt, cefBtn, action == GLFW_RELEASE, click_count);
  }
}

void BrowserView::keyPress(int key, int scanCode, int mods, int action) {
  auto vk = glfwKeyCodeToVK(key);

  CefKeyEvent evt;
  evt.is_system_key = false;
  evt.native_key_code = key;  // GLFW key code
  evt.windows_key_code = vk;

  evt.unmodified_character = MapVirtualKeyW(vk, MAPVK_VK_TO_CHAR);
  evt.character = evt.unmodified_character;

  evt.type = action == GLFW_RELEASE ? KEYEVENT_KEYUP : KEYEVENT_KEYDOWN;
  evt.modifiers = 0;
  if (mods & GLFW_MOD_SHIFT) evt.modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (mods & GLFW_MOD_CONTROL) evt.modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (mods & GLFW_MOD_ALT) evt.modifiers |= EVENTFLAG_ALT_DOWN;
  if (mods & GLFW_MOD_CAPS_LOCK) evt.modifiers |= EVENTFLAG_CAPS_LOCK_ON;
  if (mods & GLFW_MOD_NUM_LOCK) evt.modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (action == GLFW_REPEAT) evt.modifiers |= EVENTFLAG_IS_REPEAT;

  m_browser->GetHost()->SendKeyEvent(evt);
}

void BrowserView::setMenuVisible(bool vis) {
  auto msg = CefProcessMessage::Create(kSetMenuVis);
  msg->GetArgumentList()->SetBool(0, vis);
  m_browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, msg);
}