// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#include "browser/media/media_stream_devices_controller.h"

#include "browser/media/media_capture_devices_dispatcher.h"

#include "content/public/common/media_stream_request.h"

namespace brightray {

namespace {

bool HasAnyAvailableDevice() {
  const content::MediaStreamDevices& audio_devices =
      MediaCaptureDevicesDispatcher::GetInstance()->GetAudioCaptureDevices();
  const content::MediaStreamDevices& video_devices =
      MediaCaptureDevicesDispatcher::GetInstance()->GetVideoCaptureDevices();

  return !audio_devices.empty() || !video_devices.empty();
}

}  // namespace

MediaStreamDevicesController::MediaStreamDevicesController(
    const content::MediaStreamRequest& request,
    const content::MediaResponseCallback& callback)
    : request_(request),
      callback_(callback),
      // For MEDIA_OPEN_DEVICE requests (Pepper) we always request both webcam
      // and microphone to avoid popping two infobars.
      microphone_requested_(
          request.audio_type == content::MEDIA_DEVICE_AUDIO_CAPTURE ||
          request.request_type == content::MEDIA_OPEN_DEVICE),
      webcam_requested_(
          request.video_type == content::MEDIA_DEVICE_VIDEO_CAPTURE ||
          request.request_type == content::MEDIA_OPEN_DEVICE) {
}

MediaStreamDevicesController::~MediaStreamDevicesController() {
  if (!callback_.is_null()) {
    callback_.Run(content::MediaStreamDevices(),
                  /* TODO(zhm) not sure about this parameter */
                  content::MEDIA_DEVICE_INVALID_STATE,
                  scoped_ptr<content::MediaStreamUI>());
  }
}

bool MediaStreamDevicesController::TakeAction() {
  // Tab capture is allowed for extensions only and infobar is not shown for
  // extensions.
  if (request_.audio_type == content::MEDIA_TAB_AUDIO_CAPTURE ||
      request_.video_type == content::MEDIA_TAB_VIDEO_CAPTURE) {
    Deny();
    return true;
  }

  // Deny the request if the security origin is empty, this happens with
  // file access without |--allow-file-access-from-files| flag.
  if (request_.security_origin.is_empty()) {
    Deny();
    return true;
  }

  // Deny the request if there is no device attached to the OS.
  if (!HasAnyAvailableDevice()) {
    Deny();
    return true;
  }

  Accept();
  return true;
}

void MediaStreamDevicesController::Accept() {
  // Get the default devices for the request.
  content::MediaStreamDevices devices;
  if (microphone_requested_ || webcam_requested_) {
    switch (request_.request_type) {
      case content::MEDIA_OPEN_DEVICE: {
        const content::MediaStreamDevice* device = NULL;
        // For open device request pick the desired device or fall back to the
        // first available of the given type.
        if (request_.audio_type == content::MEDIA_DEVICE_AUDIO_CAPTURE) {
          device = MediaCaptureDevicesDispatcher::GetInstance()->
              GetRequestedAudioDevice(request_.requested_audio_device_id);
          // TODO(wjia): Confirm this is the intended behavior.
          if (!device) {
            device = MediaCaptureDevicesDispatcher::GetInstance()->
                GetFirstAvailableAudioDevice();
          }
        } else if (request_.video_type == content::MEDIA_DEVICE_VIDEO_CAPTURE) {
          // Pepper API opens only one device at a time.
          device = MediaCaptureDevicesDispatcher::GetInstance()->
              GetRequestedVideoDevice(request_.requested_video_device_id);
          // TODO(wjia): Confirm this is the intended behavior.
          if (!device) {
            device = MediaCaptureDevicesDispatcher::GetInstance()->
                GetFirstAvailableVideoDevice();
          }
        }
        if (device)
          devices.push_back(*device);
        break;
      } case content::MEDIA_GENERATE_STREAM: {
        bool needs_audio_device = microphone_requested_;
        bool needs_video_device = webcam_requested_;

        // Get the exact audio or video device if an id is specified.
        if (!request_.requested_audio_device_id.empty()) {
          const content::MediaStreamDevice* audio_device =
              MediaCaptureDevicesDispatcher::GetInstance()->
                  GetRequestedAudioDevice(request_.requested_audio_device_id);
          if (audio_device) {
            devices.push_back(*audio_device);
            needs_audio_device = false;
          }
        }
        if (!request_.requested_video_device_id.empty()) {
          const content::MediaStreamDevice* video_device =
              MediaCaptureDevicesDispatcher::GetInstance()->
                  GetRequestedVideoDevice(request_.requested_video_device_id);
          if (video_device) {
            devices.push_back(*video_device);
            needs_video_device = false;
          }
        }

        // If either or both audio and video devices were requested but not
        // specified by id, get the default devices.
        if (needs_audio_device || needs_video_device) {
          MediaCaptureDevicesDispatcher::GetInstance()->
              GetDefaultDevices(needs_audio_device,
                                needs_video_device,
                                &devices);
        }
        break;
      } case content::MEDIA_DEVICE_ACCESS:
        // Get the default devices for the request.
        MediaCaptureDevicesDispatcher::GetInstance()->
            GetDefaultDevices(microphone_requested_,
                              webcam_requested_,
                              &devices);
        break;
      case content::MEDIA_ENUMERATE_DEVICES:
        // Do nothing.
        NOTREACHED();
        break;
    }
  }

  content::MediaResponseCallback cb = callback_;
  callback_.Reset();

  callback_.Run(devices,
                devices.empty() ? content::MEDIA_DEVICE_NO_HARDWARE :
                                  content::MEDIA_DEVICE_OK,
                scoped_ptr<content::MediaStreamUI>());
}

void MediaStreamDevicesController::Deny() {
  content::MediaResponseCallback cb = callback_;
  callback_.Reset();
  cb.Run(content::MediaStreamDevices(),
      content::MEDIA_DEVICE_PERMISSION_DENIED,
      scoped_ptr<content::MediaStreamUI>());
}

}  // namespace brightray
