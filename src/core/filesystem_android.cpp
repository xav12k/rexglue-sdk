#include <rex/platform.h>

#if REX_PLATFORM_ANDROID

#include <rex/filesystem.h>

#include <string_view>

namespace rex::filesystem {

// Stub until the Android app shell can resolve content:// URIs through JNI.
bool IsAndroidContentUri(const std::string_view source) {
  return source.rfind("content://", 0) == 0;
}

int OpenAndroidContentFileDescriptor(const std::string_view /*uri*/, const char* /*mode*/) {
  return -1;
}

}  // namespace rex::filesystem

#endif  // REX_PLATFORM_ANDROID
