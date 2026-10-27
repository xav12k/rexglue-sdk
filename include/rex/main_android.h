#pragma once

#include <rex/platform.h>

#if REX_PLATFORM_ANDROID

#include <android/api-level.h>

namespace rex {

// API level of the device the process is running on (not the build target).
inline int GetAndroidApiLevel() {
  return android_get_device_api_level();
}

}  // namespace rex

#endif  // REX_PLATFORM_ANDROID
