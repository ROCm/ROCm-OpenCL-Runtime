/* Copyright (c) 2010 - 2021 Advanced Micro Devices, Inc.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE. */

#include "OCLPerfMapImageReadSpeed.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "CL/opencl.h"
#include "Timer.h"

// Quiet pesky warnings
#ifdef WIN_OS
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

#define NUM_SIZES 4
static const unsigned int Sizes[NUM_SIZES] = {256, 512, 1024, 2048};

#define NUM_FORMATS 1
static const cl_image_format formats[NUM_FORMATS] = {
    {CL_RGBA, CL_UNSIGNED_INT8}};
static const char *textFormats[NUM_FORMATS] = {"R8G8B8A8"};
static const unsigned int formatSize[NUM_FORMATS] = {4};

static const unsigned int Iterations[2] = {1,
                                           OCLPerfMapImageReadSpeed::NUM_ITER};

OCLPerfMapImageReadSpeed::OCLPerfMapImageReadSpeed() {
  _numSubTests = NUM_SIZES * NUM_FORMATS * 2;
}

OCLPerfMapImageReadSpeed::~OCLPerfMapImageReadSpeed() {}

static void CL_CALLBACK notify_callback(const char *errinfo,
                                        const void *private_info, size_t cb,
                                        void *user_data) {}

void OCLPerfMapImageReadSpeed::open(unsigned int test, char *units,
                                    double &conversion, unsigned int deviceId) {
  cl_uint typeOfDevice = type_;
  cl_uint numPlatforms;
  cl_platform_id platform = NULL;
  cl_uint num_devices = 0;
  cl_device_id *devices = NULL;
  cl_device_id device = NULL;
  _crcword = 0;
  conversion = 1.0f;
  _deviceId = deviceId;
  _openTest = test;

  context_ = 0;
  cmd_queue_ = 0;
  outBuffer_ = 0;
  skip_ = false;
  error_ = _wrapper->clGetPlatformIDs(0, NULL, &numPlatforms);
  CHECK_RESULT(error_ != CL_SUCCESS, "clGetPlatformIDs failed");
  if (0 < numPlatforms) {
    cl_platform_id *platforms = new cl_platform_id[numPlatforms];
    error_ = _wrapper->clGetPlatformIDs(numPlatforms, platforms, NULL);
    CHECK_RESULT(error_ != CL_SUCCESS, "clGetPlatformIDs failed");
#if 0
        // Get last for default
        platform = platforms[numPlatforms-1];
        for (unsigned i = 0; i < numPlatforms; ++i) {
#endif
    platform = platforms[_platformIndex];
    char pbuf[100];
    error_ = _wrapper->clGetPlatformInfo(platforms[_platformIndex],
                                         CL_PLATFORM_VENDOR, sizeof(pbuf), pbuf,
                                         NULL);
    num_devices = 0;
    /* Get the number of requested devices */
    error_ = _wrapper->clGetDeviceIDs(platforms[_platformIndex], typeOfDevice,
                                      0, NULL, &num_devices);
    // Runtime returns an error when no GPU devices are present instead of just
    // returning 0 devices
    // CHECK_RESULT(error_ != CL_SUCCESS, "clGetDeviceIDs failed");
    // Choose platform with GPU devices
    // if (num_devices > 0)
    //{
    //    platform = platforms[_platformIndex];
    //    break;
    //}
#if 0
        }
#endif
    delete platforms;
  }

  bufSize_ = Sizes[_openTest % NUM_SIZES];
  bufnum_ = (_openTest / NUM_SIZES) % NUM_FORMATS;
  numIter = Iterations[_openTest / (NUM_SIZES * NUM_FORMATS)];

  /*
   * If we could find our platform, use it. If not, die as we need the AMD
   * platform for these extensions.
   */
  CHECK_RESULT(platform == 0, "Couldn't find AMD platform, cannot proceed");

  devices = (cl_device_id *)malloc(num_devices * sizeof(cl_device_id));
  CHECK_RESULT(devices == 0, "no devices");

  /* Get the requested device */
  error_ = _wrapper->clGetDeviceIDs(platform, typeOfDevice, num_devices,
                                    devices, NULL);
  CHECK_RESULT(error_ != CL_SUCCESS, "clGetDeviceIDs failed");

  CHECK_RESULT(_deviceId >= num_devices, "Requested deviceID not available");
  device = devices[_deviceId];

  size_t size;
  bool imageSupport_ = false;
  error_ = _wrapper->clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT,
                            sizeof(imageSupport_), &imageSupport_, &size);
  if (!imageSupport_) {
    printf("\n%s\n", "Image not supported, skipping this test!");
    skip_ = true;
    return;
  }
  context_ = _wrapper->clCreateContext(NULL, 1, &device, notify_callback, NULL,
                                       &error_);
  CHECK_RESULT(context_ == 0, "clCreateContext failed");

  cmd_queue_ = _wrapper->clCreateCommandQueue(context_, device, 0, NULL);
  CHECK_RESULT(cmd_queue_ == 0, "clCreateCommandQueue failed");

  cl_mem_flags flags = CL_MEM_WRITE_ONLY;
  outBuffer_ = _wrapper->clCreateImage2D(context_, flags, &formats[bufnum_],
                                         bufSize_, bufSize_, 0, NULL, &error_);
  CHECK_RESULT(outBuffer_ == 0, "clCreateImage(outBuffer) failed");
}

void OCLPerfMapImageReadSpeed::run(void) {
  if(skip_) {
    return;
  }
  CPerfCounter timer;
  void *mem;

  size_t origin[3] = {0, 0, 0};
  size_t region[3] = {bufSize_, bufSize_, 1};
  size_t image_row_pitch;
  size_t image_slice_pitch;
  // Warm up
  mem = _wrapper->clEnqueueMapImage(
      cmd_queue_, outBuffer_, CL_TRUE, CL_MAP_READ, origin, region,
      &image_row_pitch, &image_slice_pitch, 0, NULL, NULL, &error_);

  CHECK_RESULT(error_, "clEnqueueMapImage failed");
  error_ = _wrapper->clEnqueueUnmapMemObject(cmd_queue_, outBuffer_, mem, 0,
                                             NULL, NULL);
  CHECK_RESULT(error_, "clEnqueueUnmapBuffer failed");
  error_ = _wrapper->clFinish(cmd_queue_);
  CHECK_RESULT(error_, "clFinish failed");

  timer.Reset();
  timer.Start();
  for (unsigned int i = 0; i < numIter; i++) {
    mem = _wrapper->clEnqueueMapImage(
        cmd_queue_, outBuffer_, CL_TRUE, CL_MAP_READ, origin, region,
        &image_row_pitch, &image_slice_pitch, 0, NULL, NULL, &error_);

    CHECK_RESULT(error_, "clEnqueueMapImage failed");
    error_ = _wrapper->clEnqueueUnmapMemObject(cmd_queue_, outBuffer_, mem, 0,
                                               NULL, NULL);
    CHECK_RESULT(error_, "clEnqueueUnmapBuffer failed");
    error_ = _wrapper->clFinish(cmd_queue_);
    CHECK_RESULT(error_, "clFinish failed");
  }

  timer.Stop();
  double sec = timer.GetElapsedTime();

  // Image map read bandwidth in GB/s
  double perf = ((double)bufSize_ * bufSize_ * formatSize[bufnum_] * numIter *
                 (double)(1e-09)) /
                sec;

  _perfInfo = (float)perf;

  char buf[256];
  SNPRINTF(buf, sizeof(buf), " (%4dx%4d) fmt:%s i: %4d (GB/s) ", bufSize_,
           bufSize_, textFormats[bufnum_], numIter);
  testDescString = buf;
}

unsigned int OCLPerfMapImageReadSpeed::close(void) {
  if (outBuffer_) {
    error_ = _wrapper->clReleaseMemObject(outBuffer_);
    CHECK_RESULT_NO_RETURN(error_ != CL_SUCCESS,
                           "clReleaseMemObject(outBuffer_) failed");
  }
  if (cmd_queue_) {
    error_ = _wrapper->clReleaseCommandQueue(cmd_queue_);
    CHECK_RESULT_NO_RETURN(error_ != CL_SUCCESS,
                           "clReleaseCommandQueue failed");
  }
  if (context_) {
    error_ = _wrapper->clReleaseContext(context_);
    CHECK_RESULT_NO_RETURN(error_ != CL_SUCCESS, "clReleaseContext failed");
  }

  return _crcword;
}
