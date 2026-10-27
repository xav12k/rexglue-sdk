#include <rex/platform.h>

#if REX_PLATFORM_ANDROID

#include <rex/thread/fiber.h>

#include <cstdlib>

// Saves callee-saved regs (x19-x30, d8-d15) on the current stack, stores sp to
// *save_sp, then loads the target's sp and restores its registers.
extern "C" void rex_fiber_switch(void** save_sp, void* next_sp);

__asm__(
    ".text\n"
    ".global rex_fiber_switch\n"
    ".type rex_fiber_switch,%function\n"
    "rex_fiber_switch:\n"
    "  sub sp, sp, #160\n"
    "  stp x19, x20, [sp, #0]\n"
    "  stp x21, x22, [sp, #16]\n"
    "  stp x23, x24, [sp, #32]\n"
    "  stp x25, x26, [sp, #48]\n"
    "  stp x27, x28, [sp, #64]\n"
    "  stp x29, x30, [sp, #80]\n"
    "  stp d8, d9, [sp, #96]\n"
    "  stp d10, d11, [sp, #112]\n"
    "  stp d12, d13, [sp, #128]\n"
    "  stp d14, d15, [sp, #144]\n"
    "  mov x9, sp\n"
    "  str x9, [x0]\n"
    "  mov sp, x1\n"
    "  ldp x19, x20, [sp, #0]\n"
    "  ldp x21, x22, [sp, #16]\n"
    "  ldp x23, x24, [sp, #32]\n"
    "  ldp x25, x26, [sp, #48]\n"
    "  ldp x27, x28, [sp, #64]\n"
    "  ldp x29, x30, [sp, #80]\n"
    "  ldp d8, d9, [sp, #96]\n"
    "  ldp d10, d11, [sp, #112]\n"
    "  ldp d12, d13, [sp, #128]\n"
    "  ldp d14, d15, [sp, #144]\n"
    "  add sp, sp, #160\n"
    "  ret\n"
    ".size rex_fiber_switch, .-rex_fiber_switch\n");

namespace rex::thread {

thread_local Fiber* Fiber::tls_current_ = nullptr;

Fiber* Fiber::ConvertCurrentThread() {
  auto* f = new Fiber();
  f->is_thread_fiber_ = true;
  tls_current_ = f;
  return f;
}

Fiber* Fiber::Create(size_t stack_size, void (*entry)(void*), void* arg) {
  auto* f = new Fiber();
  f->entry_ = entry;
  f->arg_ = arg;
  f->stack_.resize(stack_size + 16);
  uintptr_t top = (reinterpret_cast<uintptr_t>(f->stack_.data()) + f->stack_.size()) & ~uintptr_t(15);
  auto* frame = reinterpret_cast<uint64_t*>(top - 160);
  for (int i = 0; i < 20; ++i) frame[i] = 0;
  frame[11] = reinterpret_cast<uint64_t>(&Fiber::Trampoline);  // x30 slot
  f->sp_ = frame;
  return f;
}

void Fiber::Trampoline() {
  Fiber* f = tls_current_;
  f->entry_(f->arg_);
  std::abort();  // fiber entry must never return
}

void Fiber::SwitchTo(Fiber* target) {
  Fiber* from = tls_current_;
  tls_current_ = target;
  rex_fiber_switch(&from->sp_, target->sp_);
}

void Fiber::Destroy() {
  if (is_thread_fiber_) {
    tls_current_ = nullptr;
  }
  delete this;
}

}  // namespace rex::thread

#endif  // REX_PLATFORM_ANDROID
