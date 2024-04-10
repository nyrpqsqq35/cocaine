#ifndef CEF_APP_H
#define CEF_APP_H

#include <include/cef_app.h>

class FelixCefApp : public CefApp,
                    public CefBrowserProcessHandler,
                    public CefRenderProcessHandler {
public:
  FelixCefApp();

  // CefApp methods:
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
    return this;
  }

  void OnBeforeCommandLineProcessing(
      const CefString& processType,
      CefRefPtr<CefCommandLine> commandLine) override;
  void OnContextCreated(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) override;
  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) override;

private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(FelixCefApp);
};

#endif /* CEF_APP_H */
