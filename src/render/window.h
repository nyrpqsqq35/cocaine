#ifndef MENU_WIN_H
#define MENU_WIN_H

struct GLFWmonitor;
struct GLFWwindow;

namespace window {
  extern bool mousePassthrough;
  extern GLFWmonitor* monitor;
  extern GLFWwindow* window;
  extern int width;
  extern int height;
}  // namespace window

#endif /* MENU_WIN_H */
