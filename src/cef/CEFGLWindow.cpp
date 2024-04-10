// This code is a modification of the original project that can be found at
// https://github.com/if1live/cef-gl-example

#include "CEFGLWindow.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "GLCore.hpp"
#include "features/features.h"
#include "render/render.h"
#include "render/window.h"
#include "sdk/sdk.h"

#define KEY_PRESSED(key) (glfwGetKey(ptr, key) == GLFW_PRESS)
#define MOUSE_PRESSED(btn) (glfwGetMouseButton(ptr, btn) == GLFW_PRESS)

//! \brief Callback when the OpenGL base window has been resized. Dispatch this
//! event to all BrowserView.
static void reshape_callback(GLFWwindow* ptr, int w, int h) {
  assert(nullptr != ptr);
  CEFGLWindow* window =
      static_cast<CEFGLWindow*>(glfwGetWindowUserPointer(ptr));

  // Send screen size to browsers
  for (auto it : window->browsers()) {
    it->reshape(w, h);
  }
}

//! \brief Callback when the mouse has clicked inside the OpenGL base window.
//! Dispatch this event to all BrowserView.
static void mouse_callback(GLFWwindow* ptr, int btn, int action, int mods) {
  assert(nullptr != ptr);
  CEFGLWindow* window =
      static_cast<CEFGLWindow*>(glfwGetWindowUserPointer(ptr));

  // Send mouse click to browsers
  for (auto it : window->browsers()) {
    it->mouseClick(btn, action, mods);
  }
}

//! \brief Callback when the mouse has been displaced inside the OpenGL base
//! window. Dispatch this event to all BrowserView.
static void motion_callback(GLFWwindow* ptr, double x, double y) {
  assert(nullptr != ptr);
  CEFGLWindow* window =
      static_cast<CEFGLWindow*>(glfwGetWindowUserPointer(ptr));
  GLModifiers_ mod{
      .shift =
          KEY_PRESSED(GLFW_KEY_LEFT_SHIFT) || KEY_PRESSED(GLFW_KEY_RIGHT_SHIFT),
      .ctrl = KEY_PRESSED(GLFW_KEY_LEFT_CONTROL) ||
              KEY_PRESSED(GLFW_KEY_RIGHT_CONTROL),
      .alt = KEY_PRESSED(GLFW_KEY_LEFT_ALT) || KEY_PRESSED(GLFW_KEY_RIGHT_ALT),
      .leftMouse = MOUSE_PRESSED(GLFW_MOUSE_BUTTON_LEFT),
      .middleMouse = MOUSE_PRESSED(GLFW_MOUSE_BUTTON_MIDDLE),
      .rightMouse = MOUSE_PRESSED(GLFW_MOUSE_BUTTON_RIGHT)};
  // Send mouse movement to browsers
  for (auto it : window->browsers()) {
    it->mouseMove((int)x, (int)y, mod);
  }
}

//! \brief Callback when the keybaord has been pressed inside the OpenGL base
//! window. Dispatch this event to all BrowserView.
static void keyboard_callback(GLFWwindow* ptr, int key, int scanCode,
                              int action, int mods) {
  assert(nullptr != ptr);
  CEFGLWindow* window =
      static_cast<CEFGLWindow*>(glfwGetWindowUserPointer(ptr));

  // Send key press to browsers
  for (auto it : window->browsers()) {
    it->keyPress(key, scanCode, mods, action);
  }
}

CEFGLWindow::CEFGLWindow(const uint32_t width, const uint32_t height,
                         const char* title)
    : GLWindow(width, height, title) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

CEFGLWindow::~CEFGLWindow() {
  m_browsers.clear();
  CefShutdown();
}

std::weak_ptr<BrowserView> CEFGLWindow::createBrowser(const std::string& url) {
  auto web_core = std::make_shared<BrowserView>(url);
  m_browsers.push_back(web_core);
  return web_core;
}

void CEFGLWindow::removeBrowser(std::weak_ptr<BrowserView> web_core) {
  auto elem = web_core.lock();
  if (elem) {
    auto found = std::find(m_browsers.begin(), m_browsers.end(), elem);
    if (found != m_browsers.end()) {
      m_browsers.erase(found);
    }
  }
}

bool CEFGLWindow::setup() {
  auto browser = createBrowser("http://localhost:8000");
  browser.lock()->reshape(m_width, m_height);
  browser.lock()->viewport(0.0f, 0.0f, 1.0f, 1.0f);

  // Windows events
  GLCHECK(glfwSetFramebufferSizeCallback(m_window, reshape_callback));
  GLCHECK(glfwSetKeyCallback(m_window, keyboard_callback));
  GLCHECK(glfwSetCursorPosCallback(m_window, motion_callback));
  GLCHECK(glfwSetMouseButtonCallback(m_window, mouse_callback));

  // Set OpenGL states
  // GLCHECK(glViewport(0, 0, m_width, m_height));
  GLCHECK(glClearColor(0.0, 0.0, 0.0, 0.0));
  // GLCHECK(glEnable(GL_CULL_FACE));
  GLCHECK(glEnable(GL_DEPTH_TEST));
  GLCHECK(glDepthFunc(GL_LESS));
  GLCHECK(glDisable(GL_BLEND));
  GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  return true;
}

bool CEFGLWindow::update() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // ImGui::Render();
  features::esp::render();
  ImGui::EndFrame();

  // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // if (GetAsyncKeyState(VK_INSERT) & 1) {
  if (GetAsyncKeyState(keybinds::strToVK(config::gc.menuKey.key)) & 1) {
    render::mousePassthrough = !render::mousePassthrough;
    glfwSetWindowAttrib(render::window, GLFW_MOUSE_PASSTHROUGH,
                        render::mousePassthrough);
    this->m_browsers[0]->setMenuVisible(!render::mousePassthrough);

    if (render::mousePassthrough) {
      // Valorant window
      SetForegroundWindow(valorantHwnd);
    } else {
      SetForegroundWindow(glfwGetWin32Window(m_window));
    }
  }

  for (auto it : m_browsers) {
    it->draw();
  }

  CefDoMessageLoopWork();
  return true;
}
