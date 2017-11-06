//
// Copyright (c) 2008 Advanced Micro Devices, Inc. All rights reserved.
//

#include "platform/runtime.hpp"
#include "os/os.hpp"
#include "thread/thread.hpp"
#include "device/device.hpp"
#include "utils/flags.hpp"
#include "utils/options.hpp"
#include "platform/context.hpp"
#include "platform/agent.hpp"

#include "amdocl/cl_gl_amd.hpp"

#ifdef _WIN32
#include <d3d10_1.h>
#include <dxgi.h>
#include "CL/cl_d3d10.h"
#endif  //_WIN32

#if defined(_MSC_VER)  // both Win32 and Win64
#include <intrin.h>
#endif

#include <atomic>
#include <cstdlib>
#include <iostream>

namespace amd {

volatile bool Runtime::initialized_ = false;

bool Runtime::init() {
  if (initialized_) {
    return true;
  }

  // Enter a very basic critical region. We want to prevent 2 threads
  // from concurrently executing the init() routines. We can't use a
  // Monitor since the system is not yet initialized.

  static std::atomic_flag lock = ATOMIC_FLAG_INIT;
  struct CriticalRegion {
    std::atomic_flag& lock_;
    CriticalRegion(std::atomic_flag& lock) : lock_(lock) {
      while (lock.test_and_set(std::memory_order_acquire)) {
        Os::yield();
      }
    }
    ~CriticalRegion() { lock_.clear(std::memory_order_release); }
  } region(lock);

  if (initialized_) {
    return true;
  }

  if (!Flag::init() || !option::init() || !Device::init()
      // Agent initializes last
      || !Agent::init()) {
    return false;
  }

  initialized_ = true;
  return true;
}

void Runtime::tearDown() {
  if (!initialized_) {
    return;
  }

  Agent::tearDown();
  Device::tearDown();
  option::teardown();
  Flag::tearDown();
  initialized_ = false;
}

class RuntimeTearDown : public amd::HeapObject {
public:
  RuntimeTearDown() {}
  ~RuntimeTearDown() { /*Runtime::tearDown();*/ }
} runtime_tear_down;

uint ReferenceCountedObject::retain() { return ++make_atomic(referenceCount_); }

uint ReferenceCountedObject::release() {
  uint newCount = --make_atomic(referenceCount_);
  if (newCount == 0) {
    if (terminate()) {
      delete this;
    }
  }
  return newCount;
}

#ifdef _WIN32
#ifdef DEBUG
static int reportHook(int reportType, char* message, int* returnValue) {
  if (returnValue) {
    *returnValue = 1;
  }
  std::cerr << message;
  ::exit(3);
  return TRUE;
}
#endif  // DEBUG

extern "C" BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
  switch (reason) {
    case DLL_PROCESS_ATTACH:
#ifdef DEBUG
      if (!AMD_OCL_ENABLE_MESSAGE_BOX) {
        _CrtSetReportHook(reportHook);
        _set_error_mode(_OUT_TO_STDERR);
      }
#endif  // DEBUG
      break;
    case DLL_PROCESS_DETACH:
      break;
    case DLL_THREAD_DETACH: {
      amd::Thread* thread = amd::Thread::current();
      delete thread;
    } break;
    default:
      break;
  }
  return true;
}
#endif

}  // namespace amd
