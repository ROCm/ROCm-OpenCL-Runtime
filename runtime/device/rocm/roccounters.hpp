//
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
//
#ifndef ROCCOUNTERS_HPP_
#define ROCCOUNTERS_HPP_

#include "top.hpp"
#include "device/device.hpp"
#include "device/rocm/rocdevice.hpp"
#include "hsa_ven_amd_aqlprofile.h"

namespace roc {

class VirtualGPU;
class PerfCounterProfile;

//! Performance counter implementation on GPU
class PerfCounter : public device::PerfCounter {
 public:
  enum {
    ROC_UNSUPPORTED = 0,
    ROC_GFX8,
    ROC_GFX9
  };

  //! The performance counter info
  struct Info : public amd::EmbeddedObject {
    uint blockIndex_;    //!< Index of the block to configure
    uint counterIndex_;  //!< Index of the hardware counter
    uint eventIndex_;    //!< Event you wish to count with the counter
  };

  //! Constructor for the ROC PerfCounter object
  PerfCounter(const Device& device,     //!< A ROC device object
              cl_uint blockIndex,       //!< HW block index
              cl_uint counterIndex,     //!< Counter index (Counter register) within the block
              cl_uint eventIndex);      //!< Event index (Counter selection) for profiling

  //! Destructor for the ROCM PerfCounter object
  virtual ~PerfCounter();


  //! Returns the specific information about the counter
  uint64_t getInfo(uint64_t infoType            //!< The type of returned information
                   ) const;

  //! Returns the GPU device, associated with the current object
  const Device& dev() const { return roc_device_; }

  //! Returns the gfx version
  const uint32_t gfxVersion() const { return gfxVersion_; }

  //! Returns the profile reference
  PerfCounterProfile*  profileRef() const { return profileRef_; }

  //! Update the profile associated with the counter
  void  setProfile(PerfCounterProfile* profileRef);

 private:

  //! Disable default copy constructor
  PerfCounter(const PerfCounter&);

  //! Disable default operator=
  PerfCounter& operator=(const PerfCounter&);

  //! Returns the ROC performance counter descriptor
  const Info* info() const { return &info_; }

  const Device& roc_device_;   //!< The backend device
  Info info_;                 //!< The info structure for perfcounter

  hsa_ven_amd_aqlprofile_event_t event_;  //!< event information
  PerfCounterProfile*  profileRef_;   //!< perf counter profile object

  uint32_t gfxVersion_;       //!< The IP version of the device
};

//! Performance counter profile
class PerfCounterProfile : public amd::ReferenceCountedObject {
 public:
  //! Default constructor
  PerfCounterProfile(const Device& device)
    : api_({0}),
      roc_device_(device) {

    memset(&profile_, 0, sizeof(profile_));
    profile_.agent = roc_device_.getBackendDevice();
    profile_.type = HSA_VEN_AMD_AQLPROFILE_EVENT_TYPE_PMC;

    memset(&prePacket_, 0, sizeof(hsa_ext_amd_aql_pm4_packet_t));
    memset(&postPacket_, 0, sizeof(hsa_ext_amd_aql_pm4_packet_t));
    memset(&completionSignal_, 0, sizeof(hsa_signal_t));
  };

  //! Destructor for the PerfCounter profile object
  virtual ~PerfCounterProfile();

  //! Get the API tables
  bool Create() {
    hsa_agent_t agent = roc_device_.getBackendDevice();
    bool system_support, agent_support;
    hsa_system_extension_supported(HSA_EXTENSION_AMD_AQLPROFILE, 1, 0, &system_support);
    hsa_agent_extension_supported(HSA_EXTENSION_AMD_AQLPROFILE, agent, 1, 0, &agent_support);
    if (!system_support || !agent_support) {
      return false;
    }

    if (hsa_system_get_extension_table(HSA_EXTENSION_AMD_AQLPROFILE, 1, 0, &api_) != HSA_STATUS_SUCCESS) {
      return false;
    }

    return true;
  };

  //! Returns the performance counter vector
  std::vector<PerfCounter*> perfCounters() { return perfCounters_; };

  //! Add the event of performance counter object to the profile context object
  void addEvent(hsa_ven_amd_aqlprofile_event_t event) { events_.push_back(event); };

  //! Create the start packet for performance counter
  hsa_ext_amd_aql_pm4_packet_t* createStartPacket();

  //! Create the stop packet for performance counter
  hsa_ext_amd_aql_pm4_packet_t* createStopPacket();

  //! Create the profile context object
  bool initialize();  //!< HSA profile context object

  //! Return the extension API table
  const hsa_ven_amd_aqlprofile_1_00_pfn_t* api() const { return &api_; }

  //! Return the profile context object
  const hsa_ven_amd_aqlprofile_profile_t* profile() const { return &profile_; }

  //! Return the start AQL packet
  hsa_ext_amd_aql_pm4_packet_t* prePacket() { return &prePacket_; }

  //! Return the stop AQL packet
  hsa_ext_amd_aql_pm4_packet_t* postPacket() { return &postPacket_; }

 private:

  //! Disable copy constructor
  PerfCounterProfile(const PerfCounterProfile&);

  //! Disable operator=
  PerfCounterProfile& operator=(const PerfCounterProfile&);

  hsa_ven_amd_aqlprofile_1_00_pfn_t api_;   //!< The extension API table
  const Device& roc_device_;  //!< The backend device

  std::vector<PerfCounter*>  perfCounters_;    //!< Perf counters associate with the profile
  std::vector<hsa_ven_amd_aqlprofile_event_t>  events_;  //!< Events information

  hsa_ven_amd_aqlprofile_profile_t  profile_; //!< HSA profile context object
  hsa_ext_amd_aql_pm4_packet_t  prePacket_;   //!< aql packet for starting perf counter
  hsa_ext_amd_aql_pm4_packet_t  postPacket_;  //!< aql packet for stoping the perf counter

  hsa_signal_t completionSignal_;     //!< signal of completion

};

}  // namespace roc

#endif  // ROCCOUNTERS_HPP_

