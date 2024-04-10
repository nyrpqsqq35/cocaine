#include "v8handler.h"

bool FelixV8Handler::Execute(const CefString& name,
                             CefRefPtr<CefV8Value> object,
                             const CefV8ValueList& arguments,
                             CefRefPtr<CefV8Value>& retval,
                             CefString& exception) {
  if (name == kSetPointerLock) {
    if (arguments.size() != 1) {
      exception = "You are an idiot";
      return true;
    } else if (!arguments[0]->IsBool()) {
      exception = "You are still an idiot";
      return true;
    } else {
      auto pointerLock = arguments[0]->GetBoolValue();

      CefRefPtr<CefProcessMessage> msg =
          CefProcessMessage::Create(kSetPointerLock);
      msg->GetArgumentList()->SetBool(0, pointerLock);
      frame->SendProcessMessage(PID_BROWSER, msg);

      retval = CefV8Value::CreateUndefined();
      return true;
    }
  } else if (name == kSetConfig) {
    if (arguments.size() != 1) {
      exception = "Fuck you";
      return true;
    } else if (!arguments[0]->IsString()) {
      exception = " Meow";
      return true;
    } else {
      auto config = arguments[0]->GetStringValue();

      CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(kSetConfig);
      msg->GetArgumentList()->SetString(0, config);
      frame->SendProcessMessage(PID_BROWSER, msg);

      retval = CefV8Value::CreateUndefined();
      return true;
    }
  } else if (name == kSetGlobalConfig) {
    if (arguments.size() != 1) {
      exception = "Fuck you";
      return true;
    } else if (!arguments[0]->IsString()) {
      exception = " Meow";
      return true;
    } else {
      auto config = arguments[0]->GetStringValue();

      CefRefPtr<CefProcessMessage> msg =
          CefProcessMessage::Create(kSetGlobalConfig);
      msg->GetArgumentList()->SetString(0, config);
      frame->SendProcessMessage(PID_BROWSER, msg);

      retval = CefV8Value::CreateUndefined();
      return true;
    }
  }
  return false;
}