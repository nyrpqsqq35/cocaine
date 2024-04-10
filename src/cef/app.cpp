#include "app.h"

#include <cocaine.h>

#include "v8handler.h"

FelixCefApp::FelixCefApp() {}

void FelixCefApp::OnBeforeCommandLineProcessing(
    const CefString& processType, CefRefPtr<CefCommandLine> commandLine) {
  commandLine->AppendSwitchWithValue("remote-debugging-port", "9229");
  // commandLine->AppendSwitch("enable-gpu-rasterization");
  // commandLine->AppendSwitch("enable-webgl");
  commandLine->AppendSwitch("disable-webgl");
  commandLine->AppendSwitch("disable-gpu-rasterization");
  commandLine->AppendSwitch("disable-gpu-compositing");
  // commandLine->AppendSwitch("enable-begin-frame-scheduling");
}

// Renderer
void FelixCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) {
  printf("Hello context created\n");
  CefRefPtr<CefV8Value> global = context->GetGlobal();
  CefRefPtr<CefV8Handler> handler = new FelixV8Handler(browser, frame, context);
  CefRefPtr<CefV8Value> objFelix = CefV8Value::CreateObject(nullptr, nullptr);

  CefRefPtr<CefV8Value> funcLockPointer =
      CefV8Value::CreateFunction(kSetPointerLock, handler);
  objFelix->SetValue(kSetPointerLock, funcLockPointer,
                     V8_PROPERTY_ATTRIBUTE_NONE);

  CefRefPtr<CefV8Value> funcSetConfig =
      CefV8Value::CreateFunction(kSetConfig, handler);
  objFelix->SetValue(kSetConfig, funcSetConfig, V8_PROPERTY_ATTRIBUTE_NONE);

  CefRefPtr<CefV8Value> funcSetGlobalConfig =
      CefV8Value::CreateFunction(kSetGlobalConfig, handler);
  objFelix->SetValue(kSetGlobalConfig, funcSetGlobalConfig,
                     V8_PROPERTY_ATTRIBUTE_NONE);

  global->SetValue(kFelix, objFelix, V8_PROPERTY_ATTRIBUTE_NONE);
}
bool FelixCefApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
  const auto& name = message->GetName();
  auto context = frame->GetV8Context();

  if (name == kSetMenuVis) {
    context->Enter();
    auto global = context->GetGlobal();
    auto objFelix = global->GetValue(kFelix);

    auto funcSetMenuVis = objFelix->GetValue(kSetMenuVis);
    if (!funcSetMenuVis || !funcSetMenuVis->IsFunction()) return false;
    bool menuVis = message->GetArgumentList()->GetBool(0);
    CefV8ValueList args;
    args.push_back(CefV8Value::CreateBool(menuVis));
    funcSetMenuVis->ExecuteFunctionWithContext(context, nullptr, args);

    context->Exit();
    return true;
  } else if (name == kCustomMouseEvent) {
    context->Enter();
    auto args = message->GetArgumentList();
    auto mousedown = args->GetBool(0);
    auto mouseX = args->GetInt(1);
    auto mouseY = args->GetInt(2);
    auto mbtn = args->GetInt(3);

    auto global = context->GetGlobal();
    auto objFelix = global->GetValue(kFelix);
    auto funcClickMouse = objFelix->GetValue(kCustomMouseEvent);
    if (!funcClickMouse || !funcClickMouse->IsFunction()) return false;
    CefV8ValueList argz;
    argz.push_back(CefV8Value::CreateBool(mousedown));
    argz.push_back(CefV8Value::CreateInt(mouseX));
    argz.push_back(CefV8Value::CreateInt(mouseY));
    argz.push_back(CefV8Value::CreateInt(mbtn));
    funcClickMouse->ExecuteFunctionWithContext(context, nullptr, argz);
    context->Exit();
    return true;
  }
  return false;
}