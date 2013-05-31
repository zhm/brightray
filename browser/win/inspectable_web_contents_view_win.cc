#include "browser/win/inspectable_web_contents_view_win.h"

#include "browser/inspectable_web_contents_impl.h"

#include "content/public/browser/web_contents_view.h"
#include "ui/base/win/hidden_window.h"

namespace brightray {

InspectableWebContentsView* CreateInspectableContentsView(InspectableWebContentsImpl* inspectable_web_contents) {
  return new InspectableWebContentsViewWin(inspectable_web_contents);
}

InspectableWebContentsViewWin::InspectableWebContentsViewWin(InspectableWebContentsImpl* inspectable_web_contents)
    : inspectable_web_contents_(inspectable_web_contents) {
  Init(ui::GetHiddenWindow(), gfx::Rect());
  SetParent(inspectable_web_contents_->GetWebContents()->GetView()->GetNativeView(), hwnd());
}

InspectableWebContentsViewWin::~InspectableWebContentsViewWin() {
}

gfx::NativeView InspectableWebContentsViewWin::GetNativeView() const {
  return hwnd();
}

void InspectableWebContentsViewWin::ShowDevTools() {
}

void InspectableWebContentsViewWin::CloseDevTools() {
}

bool InspectableWebContentsViewWin::SetDockSide(const std::string& side) {
  return false;
}

LRESULT InspectableWebContentsViewWin::OnSize(UINT message, WPARAM, LPARAM, BOOL& handled) {
  RECT rect;
  GetClientRect(hwnd(), &rect);
  SetWindowPos(inspectable_web_contents_->GetWebContents()->GetView()->GetNativeView(),
               nullptr,
               rect.left, rect.top,
               rect.right - rect.left, rect.bottom - rect.top,
               SWP_NOZORDER);
  return 0;
}

}
