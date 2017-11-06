//
// Copyright (c) 2010 Advanced Micro Devices, Inc. All rights reserved.
//

#ifndef VERSIONS_HPP_
#define VERSIONS_HPP_

#include "utils/macros.hpp"

#ifndef AMD_PLATFORM_NAME
#define AMD_PLATFORM_NAME "AMD Accelerated Parallel Processing"
#endif  // AMD_PLATFORM_NAME

#ifndef AMD_PLATFORM_BUILD_NUMBER
#define AMD_PLATFORM_BUILD_NUMBER 2528
#endif  // AMD_PLATFORM_BUILD_NUMBER

#ifndef AMD_PLATFORM_REVISION_NUMBER
#define AMD_PLATFORM_REVISION_NUMBER 0
#endif  // AMD_PLATFORM_REVISION_NUMBER

#ifndef AMD_PLATFORM_RELEASE_INFO
#define AMD_PLATFORM_RELEASE_INFO NOT_MAINLINE(".internal")
#endif  // AMD_PLATFORM_RELEASE_INFO

#define AMD_BUILD_STRING                                                                           \
  XSTR(AMD_PLATFORM_BUILD_NUMBER)                                                                  \
  "." XSTR(AMD_PLATFORM_REVISION_NUMBER)

#ifndef AMD_PLATFORM_INFO
#define AMD_PLATFORM_INFO                                                                          \
  "AMD-APP" AMD_PLATFORM_RELEASE_INFO DEBUG_ONLY(                                                  \
      "." IF(IS_OPTIMIZED, "opt", "dbg")) " (" AMD_BUILD_STRING ")"
#endif  // ATI_PLATFORM_INFO

#endif  // VERSIONS_HPP_
