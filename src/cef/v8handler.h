#ifndef CEF_V8HANDLER_H
#define CEF_V8HANDLER_H

#include <include/cef_v8.h>

static const char* kFelix = "Felix";
static const char* kSetPointerLock = "setPointerLock";
static const char* kSetMenuVis = "setMenuVis";
static const char* kSetConfig = "setConfig";
static const char* kSetGlobalConfig = "setGlobalConfig";
static const char* kCustomMouseEvent = "customMouseEvent";

class FelixV8Handler : public CefV8Handler {
 public:
  CefRefPtr<CefBrowser> browser;
  CefRefPtr<CefFrame> frame;
  CefRefPtr<CefV8Context> context;
  FelixV8Handler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                 CefRefPtr<CefV8Context> context)
      : browser(browser), frame(frame), context(context) {}

  bool Execute(const CefString& name, CefRefPtr<CefV8Value> object,
               const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval,
               CefString& exception) override;

 private:
  IMPLEMENT_REFCOUNTING(FelixV8Handler);
};

#endif /* CEF_V8HANDLER_H */
