// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#include "common/content_client.h"

#include "common/application_info.h"

#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "content/public/common/user_agent.h"

namespace brightray {

ContentClient::ContentClient() {
}

ContentClient::~ContentClient() {
}

std::string ContentClient::GetProduct() const {
  auto name = GetApplicationName();
  base::RemoveChars(name, base::kWhitespaceASCII, &name);
  return base::StringPrintf("%s/%s",
      name.c_str(), GetApplicationVersion().c_str());
}

std::string ContentClient::GetUserAgent() const {
  return content::BuildUserAgentFromProduct(GetProduct());
}

base::StringPiece ContentClient::GetDataResource(
    int resource_id, ui::ScaleFactor scale_factor) const {
  return ui::ResourceBundle::GetSharedInstance().GetRawDataResourceForScale(
      resource_id, scale_factor);
}

gfx::Image& ContentClient::GetNativeImageNamed(int resource_id) const {
  return ui::ResourceBundle::GetSharedInstance().GetNativeImageNamed(
      resource_id);
}

}  // namespace brightray
