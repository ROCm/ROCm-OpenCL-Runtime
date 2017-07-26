//
// Copyright (c) 2008 Advanced Micro Devices, Inc. All rights reserved.
//

#include "platform/context.hpp"
#include "amdocl/cl_gl_amd.hpp"
#include "amdocl/cl_common.hpp"
#include "platform/commandqueue.hpp"

#include <algorithm>
#include <functional>

#ifdef _WIN32
#include <d3d10_1.h>
#include <dxgi.h>
#include "CL/cl_d3d10.h"
#include "CL/cl_d3d11.h"
#include "CL/cl_dx9_media_sharing.h"
#endif  //_WIN32

#if defined(_WIN32)
#include "lf.h"
#endif

namespace amd {

Context::Context(const std::vector<Device*>& devices, const Info& info)
    : devices_(devices),
      info_(info),
      properties_(NULL),
      glenv_(NULL),
      customHostAllocDevice_(NULL) {
  for (const auto& device : devices) {
    device->retain();
    if (customHostAllocDevice_ == NULL && device->customHostAllocator()) {
      customHostAllocDevice_ = device;
    }
    if (device->svmSupport()) {
      svmAllocDevice_.push_back(device);
    }
  }
  if (svmAllocDevice_.size() > 1) {
    // make sure the CPU is the last device to do allocation.
    if ((svmAllocDevice_.front()->type() == CL_DEVICE_TYPE_CPU)) {
      std::swap(svmAllocDevice_.front(), svmAllocDevice_.back());
    }

    uint isFirstDeviceFGSEnabled = svmAllocDevice_.front()->isFineGrainedSystem(true);
    for (auto& dev : svmAllocDevice_) {
      // allocation on fine - grained system incapable device first
      if (isFirstDeviceFGSEnabled && (dev->type() == CL_DEVICE_TYPE_GPU) &&
          (!(dev->isFineGrainedSystem(true)))) {
        std::swap(svmAllocDevice_.front(), dev);
        break;
      }
    }
  }
}

Context::~Context() {
  static const bool VALIDATE_ONLY = false;

  // Dissociate OCL context with any external device
  if (info_.flags_ & (GLDeviceKhr | D3D10DeviceKhr | D3D11DeviceKhr)) {
    std::vector<Device*>::const_iterator it;
    // Loop through all devices
    for (it = devices_.begin(); it != devices_.end(); it++) {
      (*it)->unbindExternalDevice(info_.flags_, info_.hDev_, info_.hCtx_, VALIDATE_ONLY);
    }
  }

  if (properties_ != NULL) {
    delete[] properties_;
  }
  if (glenv_ != NULL) {
    delete glenv_;
    glenv_ = NULL;
  }

  std::for_each(devices_.begin(), devices_.end(), std::mem_fun(&Device::release));

#if defined(_WIN32)
  lfTerminate();
#endif
}

int Context::checkProperties(const cl_context_properties* properties, Context::Info* info) {
  cl_platform_id pfmId = 0;
  uint count = 0;

  const struct Element {
    intptr_t name;
    void* ptr;
  }* p = reinterpret_cast<const Element*>(properties);

  // Clear the context infor structure
  ::memset(info, 0, sizeof(Context::Info));

  if (properties == NULL) {
    return CL_SUCCESS;
  }

  // Process all properties
  while (p->name != 0) {
    switch (p->name) {
      case CL_CONTEXT_INTEROP_USER_SYNC:
        if (p->ptr == reinterpret_cast<void*>(CL_TRUE)) {
          info->flags_ |= InteropUserSync;
        }
        break;
#ifdef _WIN32
      case CL_CONTEXT_D3D10_DEVICE_KHR:
        if (p->ptr == NULL) {
          return CL_INVALID_VALUE;
        }
        info->hDev_[D3D10DeviceKhrIdx] = p->ptr;
        info->flags_ |= D3D10DeviceKhr;
        break;
      case CL_CONTEXT_D3D11_DEVICE_KHR:
        if (p->ptr == NULL) {
          return CL_INVALID_VALUE;
        }
        info->hDev_[D3D11DeviceKhrIdx] = p->ptr;
        info->flags_ |= D3D11DeviceKhr;
        break;
      case CL_CONTEXT_ADAPTER_D3D9_KHR:
        if (p->ptr == NULL) {  // not supported for xp
          return CL_INVALID_VALUE;
        }
        info->hDev_[D3D9DeviceKhrIdx] = p->ptr;
        info->flags_ |= D3D9DeviceKhr;
        break;
      case CL_CONTEXT_ADAPTER_D3D9EX_KHR:
        if (p->ptr == NULL) {
          return CL_INVALID_VALUE;
        }
        info->hDev_[D3D9DeviceEXKhrIdx] = p->ptr;
        info->flags_ |= D3D9DeviceEXKhr;
        break;
      case CL_CONTEXT_ADAPTER_DXVA_KHR:
        if (p->ptr == NULL) {
          return CL_INVALID_VALUE;
        }
        info->hDev_[D3D9DeviceVAKhrIdx] = p->ptr;
        info->flags_ |= D3D9DeviceVAKhr;
        break;
#endif  //_WIN32

      case CL_EGL_DISPLAY_KHR:
        info->flags_ |= EGLDeviceKhr;

#ifdef _WIN32
      case CL_WGL_HDC_KHR:
#endif  //_WIN32

#if defined(__linux__)
      case CL_GLX_DISPLAY_KHR:
#endif  // linux
        info->hDev_[GLDeviceKhrIdx] = p->ptr;

#if defined(__APPLE__) || defined(__MACOSX)
      case CL_CGL_SHAREGROUP_KHR:
        Unimplemented();
        break;
#endif  //__APPLE__ || MACOS

      case CL_GL_CONTEXT_KHR:
        if (p->ptr == NULL) {
          return CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR;
        }
        if (p->name == CL_GL_CONTEXT_KHR) {
          info->hCtx_ = p->ptr;
        }
        info->flags_ |= GLDeviceKhr;
        break;
      case CL_CONTEXT_PLATFORM:
        pfmId = reinterpret_cast<cl_platform_id>(p->ptr);
        if ((NULL != pfmId) && (AMD_PLATFORM != pfmId)) {
          return CL_INVALID_VALUE;
        }
        break;
      case CL_CONTEXT_OFFLINE_DEVICES_AMD:
        if (p->ptr != reinterpret_cast<void*>(1)) {
          return CL_INVALID_VALUE;
        }
        // Set the offline device flag
        info->flags_ |= OfflineDevices;
        break;
      case CL_CONTEXT_COMMAND_INTERCEPT_CALLBACK_AMD:
        // Set the command intercept flag
        info->commandIntercept_ = (cl_int(CL_CALLBACK*)(cl_event, cl_int*))p->ptr;
        info->flags_ |= CommandIntercept;
        break;
      default:
        return CL_INVALID_VALUE;
    }
    p++;
    count++;
  }

  info->propertiesSize_ = count * sizeof(Element) + sizeof(intptr_t);
  return CL_SUCCESS;
}

int Context::create(const intptr_t* properties) {
  static const bool VALIDATE_ONLY = false;
  int result = CL_SUCCESS;

  if (properties != NULL) {
    properties_ = new cl_context_properties[info().propertiesSize_ / sizeof(cl_context_properties)];
    if (properties_ == NULL) {
      return CL_OUT_OF_HOST_MEMORY;
    }

    ::memcpy(properties_, properties, info().propertiesSize_);
  }

  // Check if OCL context can be associated with any external device
  if (info_.flags_ & (D3D10DeviceKhr | D3D11DeviceKhr | GLDeviceKhr | D3D9DeviceKhr |
                      D3D9DeviceEXKhr | D3D9DeviceVAKhr)) {
    std::vector<Device*>::const_iterator it;
    // Loop through all devices
    for (it = devices_.begin(); it != devices_.end(); it++) {
      if (!(*it)->bindExternalDevice(info_.flags_, info_.hDev_, info_.hCtx_, VALIDATE_ONLY)) {
        result = CL_INVALID_VALUE;
      }
    }
  }

  // Check if the device binding wasn't successful
  if (result != CL_SUCCESS) {
    if (info_.flags_ & GLDeviceKhr) {
      result = CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR;
    } else if (info_.flags_ & D3D10DeviceKhr) {
      // return CL_INVALID_VALUE; // FIXME_odintsov: CL_INVALID_D3D_INTEROP;
    } else if (info_.flags_ & D3D11DeviceKhr) {
      // return CL_INVALID_VALUE; // FIXME_odintsov: CL_INVALID_D3D_INTEROP;
    } else if (info_.flags_ & (D3D9DeviceKhr | D3D9DeviceEXKhr | D3D9DeviceVAKhr)) {
      // return CL_INVALID_DX9_MEDIA_ADAPTER_KHR;
    }
  } else {
    if (info_.flags_ & GLDeviceKhr) {
      // Init context for GL interop
      if (glenv_ == NULL) {
        HMODULE h = (HMODULE)Os::loadLibrary(
#ifdef _WIN32
            "OpenGL32.dll"
#else   //!_WIN32
            "libGL.so.1"
#endif  //!_WIN32
            );

        if (h && (glenv_ = new GLFunctions(h, (info_.flags_ & Flags::EGLDeviceKhr) != 0))) {
          if (!glenv_->init(reinterpret_cast<intptr_t>(info_.hDev_[GLDeviceKhrIdx]),
                            reinterpret_cast<intptr_t>(info_.hCtx_))) {
            delete glenv_;
            glenv_ = NULL;
            result = CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR;
          }
        }
      }
    }
  }
#if defined(_WIN32)
  lfInit();
#endif
  return result;
}

void* Context::hostAlloc(size_t size, size_t alignment, bool atomics) const {
  if (customHostAllocDevice_ != NULL) {
    return customHostAllocDevice_->hostAlloc(size, alignment, atomics);
  }
  return AlignedMemory::allocate(size, alignment);
}

void Context::hostFree(void* ptr) const {
  if (customHostAllocDevice_ != NULL) {
    customHostAllocDevice_->hostFree(ptr);
    return;
  }
  AlignedMemory::deallocate(ptr);
}

void* Context::svmAlloc(size_t size, size_t alignment, cl_svm_mem_flags flags) {
  unsigned int numSVMDev = svmAllocDevice_.size();
  if (numSVMDev < 1) {
    return NULL;
  }

  if (svmAllocDevice_.front()->type() == CL_DEVICE_TYPE_CPU) {
    return AlignedMemory::allocate(size, alignment);
  } else {
    void* svmPtrAlloced = NULL;
    void* tempPtr = NULL;

    for (const auto& dev : svmAllocDevice_) {
      if (dev->type() == CL_DEVICE_TYPE_GPU) {
        // check if the device support svm platform atomics,
        // skipped allocation for platform atomics if not supported by this device
        if ((flags & CL_MEM_SVM_ATOMICS) &&
            !(dev->info().svmCapabilities_ & CL_DEVICE_SVM_ATOMICS)) {
          continue;
        }
        svmPtrAlloced = dev->svmAlloc(*this, size, alignment, flags, svmPtrAlloced);
        if (svmPtrAlloced == NULL) {
          return NULL;
        }
      }
    }
    return svmPtrAlloced;
  }
}

void Context::svmFree(void* ptr) const {
  if (svmAllocDevice_.front()->type() == CL_DEVICE_TYPE_CPU) {
    AlignedMemory::deallocate(ptr);
    return;
  }

  for (const auto& dev : svmAllocDevice_) {
    if (dev->type() == CL_DEVICE_TYPE_GPU) {
      dev->svmFree(ptr);
    }
  }
  return;
}

bool Context::containsDevice(const Device* device) const {
  std::vector<Device*>::const_iterator it;

  for (it = devices_.begin(); it != devices_.end(); ++it) {
    if (device == *it || (*it)->isAncestor(device)) {
      return true;
    }
  }
  return false;
}

DeviceQueue* Context::defDeviceQueue(const Device& dev) const {
  std::map<const Device*, DeviceQueueInfo>::const_iterator it = deviceQueues_.find(&dev);
  if (it != deviceQueues_.end()) {
    return it->second.defDeviceQueue_;
  } else {
    return NULL;
  }
}

bool Context::isDevQueuePossible(const Device& dev) {
  return (deviceQueues_[&dev].deviceQueueCnt_ < dev.info().maxOnDeviceQueues_) ? true : false;
}

void Context::addDeviceQueue(const Device& dev, DeviceQueue* queue, bool defDevQueue) {
  DeviceQueueInfo& info = deviceQueues_[&dev];
  info.deviceQueueCnt_++;
  if (defDevQueue) {
    info.defDeviceQueue_ = queue;
  }
}

void Context::removeDeviceQueue(const Device& dev, DeviceQueue* queue) {
  DeviceQueueInfo& info = deviceQueues_[&dev];
  assert((info.deviceQueueCnt_ != 0) && "The device queue map is empty!");
  info.deviceQueueCnt_--;
  if (info.defDeviceQueue_ == queue) {
    info.defDeviceQueue_ = NULL;
  }
}

}  // namespace amd
