//
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
//

#include "device/rocm/roccounters.hpp"
#include "device/rocm/rocvirtual.hpp"
#include <array>


hsa_status_t PerfCounterCallback(
  hsa_ven_amd_aqlprofile_info_type_t  info_type,
  hsa_ven_amd_aqlprofile_info_data_t* info_data,
  void* callback_data)
{
  typedef std::vector<hsa_ven_amd_aqlprofile_info_data_t> passed_data_t;

  if (info_type == HSA_VEN_AMD_AQLPROFILE_INFO_PMC_DATA) {
    reinterpret_cast<passed_data_t*>(callback_data)->push_back(*info_data);
  }

  return HSA_STATUS_SUCCESS;
}


namespace roc {

/*
 Converting from ORCA cmndefs.h to ROCR hsa_ven_amd_aqlprofile.h
 Note that some blocks are not defined in cmndefs.h
*/

static const std::array<std::pair<hsa_ven_amd_aqlprofile_block_name_t, int>, 97> viBlockIdOrcaToRocr = {{
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // CB0 - 0
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 1},      // CB1 - 1
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 2},      // CB2 - 2
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 3},      // CB3 - 3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_CPF, 0},     // CPF - 4
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // DB0 - 5
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 1},      // DB1 - 6
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 2},      // DB2 - 7
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 3},      // DB3 - 8
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_GRBM, 0},    // GRBM - 9
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_GRBMSE, 0},  // GRBMSE - 10
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // PA_SU - 11
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // PA_SC - 12
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SPI, 0},     // SPI - 13
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SQ, 0},      // SQ - 14
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_ES - 15
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_GS - 16
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_VS - 17
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_PS - 18
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_LS - 19
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_HS - 20
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SQCS, 0},    // SQ_CS - 21
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SX, 0},      // SX - 22
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0},      // TA0 - 23
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 1},      // TA1 - 24
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 2},      // TA2 - 25
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 3},      // TA3 - 26
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 4},      // TA4 - 27
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 5},      // TA5 - 28
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 6},      // TA6 - 29
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 7},      // TA7 - 30
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 8},      // TA8 - 31
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 9},      // TA9 - 32
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0a},   // TA10 - 33
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0b},   // TA11 - 34
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0c},   // TA12 - 35
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0d},   // TA13 - 36
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0e},   // TA14 - 37
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0f},   // TA15 - 38
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCA, 0},     // TCA0 - 39
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCA, 1},     // TCA1 - 40
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0},     // TCC0 - 41
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 1},     // TCC1 - 42
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 2},     // TCC2 - 43
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 3},     // TCC3 - 44
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 4},     // TCC4 - 45
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 5},     // TCC5 - 46
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 6},     // TCC6 - 47
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 7},     // TCC7 - 48
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 8},     // TCC8 - 49
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 9},     // TCC9 - 50
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0a},  // TCC10 - 51
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0b},  // TCC11 - 52
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0c},  // TCC12 - 53
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0d},  // TCC13 - 54
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0e},  // TCC14 - 55
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0f},  // TCC15 - 56
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0},      // TD0 - 57
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 1},      // TD1 - 58
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 2},      // TD2 - 59
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 3},      // TD3 - 60
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 4},      // TD4 - 61
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 5},      // TD5 - 62
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 6},      // TD6 - 63
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 7},      // TD7 - 64
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 8},      // TD8 - 65
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 9},      // TD9 - 66
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0a},   // TD10 - 67
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0b},   // TD11 - 68
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0c},   // TD12 - 69
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0d},   // TD13 - 70
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0e},   // TD14 - 71
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0f},   // TD15 - 72
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0},     // TCP0 - 73
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 1},     // TCP1 - 74
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 2},     // TCP2 - 75
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 3},     // TCP3 - 76
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 4},     // TCP4 - 77
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 5},     // TCP5 - 78
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 6},     // TCP6 - 79
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 7},     // TCP7 - 80
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 8},     // TCP8 - 81
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 9},     // TCP9 - 82
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0a},  // TCP10 - 83
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0b},  // TCP11 - 84
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0c},  // TCP12 - 85
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0d},  // TCP13 - 86
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0e},  // TCP14 - 87
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0f},  // TCP15 - 88
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_GDS, 0},     // GDS - 89
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // VGT - 90
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // IA - 91
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_MCSEQ, 0},   // MC - 92
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SRBM, 0},    // SRBM - 93
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // WD - 94
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // CPG - 95
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_CPC, 0},     // CPC - 96
}};

// The number of counters per block has been increased for gfx9 but this table may not reflect all
// of them
// as compute may not use all of them.
static const std::array<std::pair<hsa_ven_amd_aqlprofile_block_name_t, int>, 125> gfx9BlockIdOrcaToRocr = {{
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // CB0
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 1},      // CB1
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 2},      // CB2
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 3},      // CB3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_CPF, 0},     // CPF
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // DB0
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 1},      // DB1
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 2},      // DB2
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 3},      // DB3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_GRBM, 0},    // GRBM
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_GRBMSE, 0},  // GRBMSE
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // PA_SU
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // PA_SC
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SPI, 0},     // SPI
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SQ, 0},      // SQ
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_ES
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_GS
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_VS
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_PS
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_LS
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // SQ_HS
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SQCS, 0},    // SQ_CS
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_SX, 0},      // SX
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0},      // TA0
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 1},      // TA1
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 2},      // TA2
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 3},      // TA3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 4},      // TA4
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 5},      // TA5
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 6},      // TA6
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 7},      // TA7
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 8},      // TA8
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 9},      // TA9
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0a},   // TA10
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0b},   // TA11
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0c},   // TA12
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0d},   // TA13
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0e},   // TA14
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TA, 0x0f},   // TA15
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCA, 0},     // TCA0
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCA, 1},     // TCA1
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0},     // TCC0
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 1},     // TCC1
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 2},     // TCC2
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 3},     // TCC3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 4},     // TCC4
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 5},     // TCC5
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 6},     // TCC6
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 7},     // TCC7
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 8},     // TCC8
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 9},     // TCC9
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0a},  // TCC10
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0b},  // TCC11
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0c},  // TCC12
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0d},  // TCC13
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0e},  // TCC14
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCC, 0x0f},  // TCC15
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0},      // TD0
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 1},      // TD1
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 2},      // TD2
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 3},      // TD3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 4},      // TD4
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 5},      // TD5
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 6},      // TD6
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 7},      // TD7
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 8},      // TD8
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 9},      // TD9
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0a},   // TD10
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0b},   // TD11
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0c},   // TD12
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0d},   // TD13
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0e},   // TD14
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TD, 0x0f},   // TD15
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0},     // TCP0
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 1},     // TCP1
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 2},     // TCP2
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 3},     // TCP3
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 4},     // TCP4
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 5},     // TCP5
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 6},     // TCP6
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 7},     // TCP7
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 8},     // TCP8
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 9},     // TCP9
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0a},  // TCP10
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0b},  // TCP11
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0c},  // TCP12
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0d},  // TCP13
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0e},  // TCP14
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_TCP, 0x0f},  // TCP15
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_GDS, 0},     // GDS - 89
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // VGT - 90
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // IA - 91
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // WD - 92
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // CPG - 93
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_CPC, 0},     // CPC - 94
// blocks that are not defined in GSL
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // UNUSED - 95
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // UNUSED - 96
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // UNUSED - 97
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_ATCL2, 0},   // ATCL2  - 98
    {HSA_VEN_AMD_AQLPROFILE_BLOCK_NAME_MCVML2, 0},  // MCVML2 - 99
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},      // EA - 100
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 1},      // EA - 101
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 2},      // EA - 102
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 3},      // EA - 103
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 4},      // EA - 104
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 5},      // EA - 105
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 6},      // EA - 106
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 7},      // EA - 107
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 8},      // EA - 108
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 9},      // EA - 109
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0x0a},   // EA - 110
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0x0b},   // EA - 111
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0x0c},   // EA - 112
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0x0d},   // EA - 113
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0x0e},   // EA - 114
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0x0f},   // EA - 115
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},     // RPB - 116
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 0},     // RMI - 117
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 1},     // RMI - 118
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 2},     // RMI - 119
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 3},     // RMI - 120
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 4},     // RMI - 121
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 5},     // RMI - 122
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 6},     // RMI - 123
    {HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER, 7},     // RMI - 124
}};


  //! Constructor for the ROC PerfCounter object
PerfCounter::PerfCounter(const Device& device,   //!< A ROC device object
  cl_uint blockIndex,     //!< HW block index
  cl_uint counterIndex,   //!< Counter index (Counter register) within the block
  cl_uint eventIndex)     //!< Event index (Counter selection) for profiling
      : roc_device_(device),
        profileRef_(nullptr) {

  info_.blockIndex_ = blockIndex;       // Block name + block index
  info_.counterIndex_ = counterIndex;   // Ignored as not being used in PPT library
  info_.eventIndex_ = eventIndex;       // Counter Event Selection (counter_id)

  // these block indices are valid for the SI (Gfx8) & Gfx9 devices
  switch (roc_device_.deviceInfo().gfxipVersion_ / 100) {
    case (8):
      gfxVersion_ = ROC_GFX8;
      if (blockIndex < viBlockIdOrcaToRocr.size()) {
        auto p = viBlockIdOrcaToRocr[blockIndex];
        event_.block_name = std::get<0>(p);
        event_.block_index = std::get<1>(p);
      }
      break;
    case (9):
      gfxVersion_ = ROC_GFX9;
      if (blockIndex < gfx9BlockIdOrcaToRocr.size()) {
        auto p = gfx9BlockIdOrcaToRocr[blockIndex];
        event_.block_name = std::get<0>(p);
        event_.block_index = std::get<1>(p);
      }
      break;
    default:
      gfxVersion_ = ROC_UNSUPPORTED;
      event_.block_name = HSA_VEN_AMD_AQLPROFILE_BLOCKS_NUMBER;
      event_.block_index = 0;
      break;
  }
  event_.counter_id = eventIndex;
}

void PerfCounter::setProfile(PerfCounterProfile* profileRef) {
  profileRef->perfCounters().push_back(this);
  profileRef->addEvent(event_);

  if (profileRef_ != nullptr) {
    profileRef_->release();
  }
  profileRef_ = profileRef;
  profileRef->retain();
}

uint64_t PerfCounter::getInfo(uint64_t infoType) const {
  switch (infoType) {
    case CL_PERFCOUNTER_GPU_BLOCK_INDEX: {
      // Return the GPU block index
      return info()->blockIndex_;
    }
    case CL_PERFCOUNTER_GPU_COUNTER_INDEX: {
      // Return the GPU counter index
      return info()->counterIndex_;
    }
    case CL_PERFCOUNTER_GPU_EVENT_INDEX: {
      // Return the GPU event index
      return info()->eventIndex_;
    }
    case CL_PERFCOUNTER_DATA: {

      const hsa_ven_amd_aqlprofile_profile_t* profile = profileRef_->profile();

      std::vector<hsa_ven_amd_aqlprofile_info_data_t> data;
      profileRef_->api()->hsa_ven_amd_aqlprofile_iterate_data(profile,
                                                              PerfCounterCallback,
                                                              &data);

      uint64_t result = 0;
      std::vector<hsa_ven_amd_aqlprofile_info_data_t>::iterator it;
      for (it = data.begin(); it != data.end(); ++it) {
        if (it->pmc_data.event.block_name == event_.block_name &&
            it->pmc_data.event.block_index == event_.block_index &&
            it->pmc_data.event.counter_id == event_.counter_id) {
            result += it->pmc_data.result;
        }
      }
      return result;
    }
    default:
      LogError("Wrong PerfCounter::getInfo parameter");
  }
  return 0;
}

PerfCounter::~PerfCounter() {

  if (profileRef_ != nullptr) {
    profileRef_->release();
    profileRef_ = nullptr;
  }
}


bool PerfCounterProfile::initialize() {

  uint32_t  cmd_buf_size;
  uint32_t  out_buf_size;

  if (api_.hsa_ven_amd_aqlprofile_get_info(
            &profile_,
            HSA_VEN_AMD_AQLPROFILE_INFO_COMMAND_BUFFER_SIZE,
            &cmd_buf_size) != HSA_STATUS_SUCCESS) {
    return false;
  }

  if (api_.hsa_ven_amd_aqlprofile_get_info(
            &profile_,
            HSA_VEN_AMD_AQLPROFILE_INFO_PMC_DATA_SIZE,
            &out_buf_size) != HSA_STATUS_SUCCESS) {
    return false;
  }

  const uint32_t alignment = amd::Os::pageSize();     // use page alignment

  hsa_ven_amd_aqlprofile_descriptor_t cmd_buf = profile_.command_buffer;
  if (cmd_buf.ptr != nullptr && cmd_buf.size != cmd_buf_size) {
    roc_device_.memFree(cmd_buf.ptr, cmd_buf.size);
    cmd_buf.ptr = nullptr;
  }

  if (cmd_buf.ptr == nullptr) {
    void *buf_ptr = roc_device_.hostAlloc(cmd_buf_size, alignment, 1);
    if (buf_ptr != nullptr) {
      cmd_buf.ptr = buf_ptr;
      cmd_buf.size = cmd_buf_size;
      profile_.command_buffer = cmd_buf;
    }
    else {
      return false;
    }
  }

  hsa_ven_amd_aqlprofile_descriptor_t out_buf = profile_.output_buffer;
  if (out_buf.ptr != nullptr && out_buf.size != out_buf_size) {
    roc_device_.memFree(out_buf.ptr, out_buf.size);
    out_buf.ptr = nullptr;
  }

  if (out_buf.ptr == nullptr) {
    void *buf_ptr = roc_device_.hostAlloc(out_buf_size, alignment, 1);
    if (buf_ptr != nullptr) {
      out_buf.ptr = buf_ptr;
      out_buf.size = out_buf_size;
      profile_.output_buffer = out_buf;
    }
    else {
      roc_device_.hostFree(cmd_buf.ptr, cmd_buf.size);
      return false;
    }
  }

  // create the completion signal
  if (hsa_signal_create(1, 0, nullptr, &completionSignal_) != HSA_STATUS_SUCCESS) {
    return false;
  }

  return true;
}

hsa_ext_amd_aql_pm4_packet_t* PerfCounterProfile::createStartPacket() {

  profile_.events = &events_[0];
  profile_.event_count = events_.size();

  // set up the profile aql packets for capturing performance counter
  if (api_.hsa_ven_amd_aqlprofile_start(&profile_, &prePacket_) != HSA_STATUS_SUCCESS) {
    return nullptr;
  }

  return &prePacket_;
}

hsa_ext_amd_aql_pm4_packet_t* PerfCounterProfile::createStopPacket() {

  profile_.events = &events_[0];
  profile_.event_count = events_.size();

  // set up the profile aql packets for post-capturing performance counter
  // and create the completion signal
  if (api_.hsa_ven_amd_aqlprofile_stop(&profile_, &postPacket_) != HSA_STATUS_SUCCESS) {
    return nullptr;
  }

  postPacket_.completion_signal = completionSignal_;

  return &postPacket_;
}

PerfCounterProfile::~PerfCounterProfile() {

  if (completionSignal_.handle != 0) {
    hsa_signal_destroy(completionSignal_);
  }

  if (profile_.command_buffer.ptr) {
    roc_device_.memFree(profile_.command_buffer.ptr, profile_.command_buffer.size);
  }

  if (profile_.output_buffer.ptr) {
    roc_device_.memFree(profile_.output_buffer.ptr, profile_.output_buffer.size);
  }
}

}  // namespace roc

