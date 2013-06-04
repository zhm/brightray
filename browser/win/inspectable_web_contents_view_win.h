#ifndef BRIGHTRAY_BROWSER_WIN_INSPECTABLE_WEB_CONTENTS_VIEW_WIN_H_
#define BRIGHTRAY_BROWSER_WIN_INSPECTABLE_WEB_CONTENTS_VIEW_WIN_H_

#include "browser/inspectable_web_contents_view.h"

#include "base/compiler_specific.h"
#include "ui/base/win/window_impl.h"

namespace brightray {

class InspectableWebContentsImpl;

class InspectableWebContentsViewWin
    : public InspectableWebContentsView,
      public ui::WindowImpl {
public:
  InspectableWebContentsViewWin(InspectableWebContentsImpl*);
  ~InspectableWebContentsViewWin();

  BEGIN_MSG_MAP_EX(InspectableWebContentsViewWin)
    MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnSize)
  END_MSG_MAP()

  virtual gfx::NativeView GetNativeView() const OVERRIDE;
  virtual void ShowDevTools() OVERRIDE;
  virtual void CloseDevTools() OVERRIDE;
  virtual bool SetDockSide(const std::string& side) OVERRIDE;

  InspectableWebContentsImpl* inspectable_web_contents() { return inspectable_web_contents_; }

private:
  void UpdateViews();

  LRESULT OnSize(UINT message, WPARAM, LPARAM, BOOL& handled);

  // Owns us.
  InspectableWebContentsImpl* inspectable_web_contents_;
  bool devtools_docked_;

  DISALLOW_COPY_AND_ASSIGN(InspectableWebContentsViewWin);
};

}

#endif
