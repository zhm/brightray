#include "browser/win/inspectable_web_contents_view_win.h"

#include "browser/inspectable_web_contents_impl.h"

#include "content/public/browser/web_contents_view.h"
#include "ui/base/win/hidden_window.h"

namespace brightray {

InspectableWebContentsView* CreateInspectableContentsView(InspectableWebContentsImpl* inspectable_web_contents) {
  return new InspectableWebContentsViewWin(inspectable_web_contents);
}

InspectableWebContentsViewWin::InspectableWebContentsViewWin(InspectableWebContentsImpl* inspectable_web_contents)
    : inspectable_web_contents_(inspectable_web_contents),
      devtools_docked_(false) {
  Init(ui::GetHiddenWindow(), gfx::Rect());
  SetParent(inspectable_web_contents_->GetWebContents()->GetView()->GetNativeView(), hwnd());
}

InspectableWebContentsViewWin::~InspectableWebContentsViewWin() {
}

gfx::NativeView InspectableWebContentsViewWin::GetNativeView() const {
  return hwnd();
}

void InspectableWebContentsViewWin::ShowDevTools() {
  auto devtools_hwnd = inspectable_web_contents_->devtools_web_contents()->GetView()->GetNativeView();
  SetParent(devtools_hwnd, hwnd());
  devtools_docked_ = true;
  UpdateViews();
}

void InspectableWebContentsViewWin::CloseDevTools() {
  devtools_docked_ = false;
}

bool InspectableWebContentsViewWin::SetDockSide(const std::string& side) {
  return false;
}

void InspectableWebContentsViewWin::UpdateViews() {
  RECT win_rect;
  GetClientRect(hwnd(), &win_rect);
  gfx::Rect rect(win_rect);

  gfx::Rect contents_rect = rect;
  gfx::Rect devtools_rect = rect;

  if (devtools_docked_) {
    contents_rect.set_height(rect.height() * 2 / 3);

    devtools_rect.set_height(rect.height() - contents_rect.height());
    devtools_rect.set_y(contents_rect.bottom());
  }

  auto devtools = inspectable_web_contents_->devtools_web_contents();
  auto handle = BeginDeferWindowPos(devtools ? 2 : 1);
  handle = DeferWindowPos(handle, inspectable_web_contents_->GetWebContents()->GetView()->GetNativeView(),
                          nullptr,
                          contents_rect.x(), contents_rect.y(), contents_rect.width(), contents_rect.height(), SWP_NOZORDER);
  if (devtools) {
    if (devtools_docked_) {
      handle = DeferWindowPos(handle, devtools->GetView()->GetNativeView(),
                              nullptr,
                              devtools_rect.x(), devtools_rect.y(), devtools_rect.width(), devtools_rect.height(), SWP_NOZORDER | SWP_SHOWWINDOW);
    } else {
      handle = DeferWindowPos(handle, devtools->GetView()->GetNativeView(),
                              nullptr,
                              0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
    }
  }
  EndDeferWindowPos(handle);
}

LRESULT InspectableWebContentsViewWin::OnSize(UINT message, WPARAM, LPARAM, BOOL& handled) {
  UpdateViews();
  return 0;
}

}
