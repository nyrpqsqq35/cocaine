#include <ShlObj_core.h>
#include <cocaine.h>

#include <filesystem>

#include "bmain.h"
#include "cef/CEFGLWindow.hpp"
#include "cef/app.h"
#include "hid/hid.h"
#include "render/render.h"
#include "render/window.h"

void ay() {
  // hid::reset();
  // hid::cmd->operation = hid::Operation::KeyPressAndRelease;
  // hid::cmd->keyPressAndRelease.ch = 'h';
  // hid::cmd->keyPressAndRelease.delay = 150;
  // hid::send();
}

std::filesystem::path getAppDataPath() {
  PWSTR pathTmp_;
  HRESULT hr =
      SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp_);
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Could not get RoamingAppData path");
  return std::filesystem::path(pathTmp_);
}

static void CEFsetUp(int argc, char** argv) {
  CefRefPtr<FelixCefApp> app(new FelixCefApp);

  CefMainArgs args(::GetModuleHandle(nullptr));
  int exit_code = CefExecuteProcess(args, app.get(), nullptr);
  if (exit_code >= 0) {
    exit(exit_code);
  } else if (exit_code == -1) {
    // If called for the browser process (identified by no "type" command-line
    // value) it will return immediately with a value of -1
  }

  // Configurate Chromium
  CefSettings settings;
  auto appDataPath = getAppDataPath() / "cocaine";
  // CefString(&settings.locales_dir_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/locales";
  // CefString(&settings.resources_dir_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/";
  // CefString(&settings.framework_dir_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/";
  CefString(&settings.cache_path) = appDataPath / "cache";
  CefString(&settings.user_data_path) = appDataPath / "userdata";
  settings.windowless_rendering_enabled = true;
#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  bool result = CefInitialize(args, settings, app.get(), nullptr);
  if (!result) {
    std::cerr << "CefInitialize: failed" << std::endl;
    exit(-2);
  }
}

int main(int argc, char** argv) {
  try {
    config::setup();
    CEFsetUp(argc, argv);
    hid::setup();

    // We're in the main browser process
    CEFGLWindow win(0, 0, "Microsoft PowerPoint 2022");
    bmain();
    int res = win.start();

    render::shutdown();
    hid::shutdown();

    return res;
  } catch (std::exception& ex) {
    fprintf(stderr, "Error! %s\n", ex.what());
    DebugBreak();
    return 1;
  }
}