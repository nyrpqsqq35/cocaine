#include <cocaine.h>
#include <gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace window {
  bool mousePassthrough = false;
  GLFWmonitor* monitor = nullptr;
  GLFWwindow* window = nullptr;
  int width = 0;
  int height = 0;
}  // namespace window
