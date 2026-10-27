#include <android/native_window.h>

#include <rex/ui/surface_android.h>

namespace rex {
namespace ui {

bool AndroidNativeWindowSurface::GetSizeImpl(uint32_t& width_out, uint32_t& height_out) const {
  if (!window_) {
    return false;
  }
  int32_t w = ANativeWindow_getWidth(window_);
  int32_t h = ANativeWindow_getHeight(window_);
  if (w <= 0 || h <= 0) {
    return false;
  }
  width_out = uint32_t(w);
  height_out = uint32_t(h);
  return true;
}

}  // namespace ui
}  // namespace rex
