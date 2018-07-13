/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef __MPAM_HWREG_H__
#define __MPAM_HWREG_H__

/*******************************************************************************
 * MPAM system register bit definitions & constants
 ******************************************************************************/
/* MPAMn_ELx bit definitions */
#define MPAMn_ELx_PARTID_I_SHIFT    0
#define MPAMn_ELx_PARTID_D_SHIFT    16
#define MPAMn_ELx_PMG_I_SHIFT       32
#define MPAMn_ELx_PMG_D_SHIFT       40
#define MPAMn_ELx_MPAMEN_SHIFT      63

#define MPAMn_ELx_PARTID_I_MASK     0xffff
#define MPAMn_ELx_PARTID_D_MASK     0xffff
#define MPAMn_ELx_PMG_I_MASK        0xff
#define MPAMn_ELx_PMG_D_MASK        0xff
#define MPAMn_ELx_MPAMEN_MASK       0x1

/* MPAMIDR_EL1 bit definitions */
#define MPAMIDR_PARTID_MAX_SHIFT    0
#define MPAMIDR_PMG_MAX_SHIFT       32

#define MPAMIDR_PARTID_MAX_MASK     0xffff
#define MPAMIDR_PMG_MAX_MASK        0xff

/*******************************************************************************
 * MPAM memory mapped ID register offsets & constants
 ******************************************************************************/
#define REG_MPAMF_IDR               0x0000
#define REG_MPAMF_SIDR              0x0008
#define REG_MPAMF_MSMON_IDR         0x0080
#define REG_MPAMF_IMPL_IDR          0x0028
#define REG_MPAMF_CPOR_IDR          0x0030
#define REG_MPAMF_CCAP_IDR          0x0038
#define REG_MPAMF_MBW_IDR           0x0040
#define REG_MPAMF_PRI_IDR           0x0048
#define REG_MPAMF_CSUMON_IDR        0x0088
#define REG_MPAMF_MBWUMON_IDR       0x0090
#define REG_PARTID_NRW_IDR          0x0050
#define REG_MPAMF_IIDR              0x0018
#define REG_MPAMF_AIDR              0x0020

/* MPAMF_IDR bit definitions */
#define IDR_PARTID_MAX_SHIFT        0
#define IDR_PMG_MAX_SHIFT           16
#define IDR_HAS_CCAP_PART_SHIFT     24
#define IDR_HAS_CPOR_PART_SHIFT     25
#define IDR_HAS_MBW_PART_SHIFT      26
#define IDR_HAS_MSMON_SHIFT         30

#define IDR_PARTID_MAX_MASK         0xffff
#define IDR_PMG_MAX_MASK            0xff
#define IDR_HAS_CCAP_PART_MASK      0x1
#define IDR_HAS_CPOR_PART_MASK      0x1
#define IDR_HAS_MBW_PART_MASK       0x1
#define IDR_HAS_MSMON_MASK          0x1

/* MPAMF_MSMON_IDR bit definitions */
#define MSMON_IDR_MSMON_CSU_SHIFT           16
#define MSMON_IDR_MSMON_MBWU_SHIFT          17
#define MSMON_IDR_HAS_LOCAL_CAPT_EVNT_SHIFT 31

#define MSMON_IDR_MSMON_CSU_MASK            0x1
#define MSMON_IDR_MSMON_MBWU_MASK           0x1
#define MSMON_IDR_HAS_LOCAL_CAPT_EVNT_MASK  0x1

/* MPAMF_CPOR_IDR bit definitions */
#define CPOR_IDR_CPBM_WD_SHIFT      0
#define CPOR_IDR_CPBM_WD_MASK       0xffff

/* MPAMF_CCAP_IDR bit definitions */
#define CCAP_IDR_CMAX_WD_SHIFT      0
#define CCAP_IDR_CMAX_WD_MASK       0x3f

/* MPAMF_MBW_IDR bit definitions */
#define MBW_IDR_BWA_WD_SHIFT        0
#define MBW_IDR_HAS_MIN_SHIFT       10
#define MBW_IDR_HAS_MAX_SHIFT       11
#define MBW_IDR_HAS_PBM_SHIFT       12
#define MBW_IDR_HAS_WINDWR_SHIFT    14
#define MBW_IDR_BWPBM_WD_SHIFT      16

#define MBW_IDR_BWA_WD_MASK         0x3f
#define MBW_IDR_HAS_MIN_MASK        0x1
#define MBW_IDR_HAS_MAX_MASK        0x1
#define MBW_IDR_HAS_PBM_MASK        0x1
#define MBW_IDR_HAS_WINDWR_MASK     0x1
#define MBW_IDR_BWPBM_WD_MASK       0x1fff

/* MPAMF_CSUMON_IDR bit definitions */
#define CSUMON_IDR_NUM_MON_SHIFT        0
#define CSUMON_IDR_HAS_CAPTURE_SHIFT    31

#define CSUMON_IDR_NUM_MON_MASK         0xffff
#define CSUMON_IDR_HAS_CAPTURE_MASK     0x1

/* MPAMF_MBWUMON_IDR bit definitions */
#define MBWUMON_IDR_NUM_MON_SHIFT       0
#define MBWUMON_IDR_HAS_CAPTURE_SHIFT   31

#define MBWUMON_IDR_NUM_MON_MASK        0xffff
#define MBWUMON_IDR_HAS_CAPTURE_MASK    0x1

/* MPAMF_AIDR bit definitions */
#define AIDR_ARCH_MIN_VER_SHIFT     0
#define AIDR_ARCH_MAJ_VER_SHIFT     4

#define AIDR_ARCH_MIN_VER_MASK      0xf
#define AIDR_ARCH_MAJ_VER_MASK      0xf

/*******************************************************************************
 * MPAM memory mapped partitioning config register offsets & constants
 ******************************************************************************/
#define REG_MPAMCFG_PART_SEL        0x0100
#define REG_MPAMCFG_CPBM            0x1000
#define REG_MPAMCFG_CMAX            0x0108
#define REG_MPAMCFG_MBW_MIN         0x0200
#define REG_MPAMCFG_MBW_MAX         0x0208
#define REG_MPAMCFG_MBW_WINWD       0x0220
#define REG_MPAMCFG_MBW_PBM         0x2000

/* MPAMCFG_PART_SEL bit definitions */
#define PART_SEL_PARTID_SEL_SHIFT   0
#define PART_SEL_INTERNAL_SHIFT     16

#define PART_SEL_PARTID_SEL_MASK    0xffff
#define PART_SEL_INTERNAL_MASK      0x1

/* MPAMCFG_MBW_MAX bit definitions */
#define MBW_MAX_HARDLIM_SHIFT       31
#define MBW_MAX_HARDLIM_MASK        0x1

/* MPAMCFG_MBW_WINWD bit definitions */
#define MBW_WINWD_US_FRAC_SHIFT     0
#define MBW_WINWD_US_INT_SHIFT      8

#define MBW_WINWD_US_FRAC_MASK      0xff
#define MBW_WINWD_US_INT_MASK       0xffff

/*******************************************************************************
 * MPAM memory mapped monitoring config register offsets & constants
 ******************************************************************************/
#define REG_MSMON_CFG_MON_SEL       0x0800
#define REG_MSMON_CFG_CSU_FLT       0x0810
#define REG_MSMON_CFG_CSU_CTL       0x0818
#define REG_MSMON_CFG_MBWU_FLT      0x0820
#define REG_MSMON_CFG_MBWU_CTL      0x0828
#define REG_MSMON_CSU               0x0840
#define REG_MSMON_CSU_CAPTURE       0x0848
#define REG_MSMON_MBWU              0x0860
#define REG_MSMON_MBWU_CAPTURE      0x0868
#define REG_MSMON_CAPT_EVNT         0x0808

/* MSMON_CFG_MON_SEL bit definitions */
#define MON_SEL_MON_SEL_SHIFT     0
#define MON_SEL_MON_SEL_MASK      0xffff

/* MSMON_CFG_CSU_FLT bit definitions */
#define CSU_FLT_PARTID_SHIFT    0
#define CSU_FLT_PMG_SHIFT       16

#define CSU_FLT_PARTID_MASK     0xffff
#define CSU_FLT_PMG_MASK        0xff

/* MSMON_CFG_CSU_CTL bit definitions */
#define CSU_CTL_TYPE_SHIFT          0
#define CSU_CTL_OFLOW_STATUS_SHIFT  26

#define CSU_CTL_TYPE_MASK           0xff
#define CSU_CTL_OFLOW_STATUS_MASK   0x1

#define CSU_CTL_ENABLE_MATCH_PARTID_SHIFT   16
#define CSU_CTL_ENABLE_MATCH_PMG_SHIFT      17
#define CSU_CTL_ENABLE_OFLOW_FRZ_SHIFT      24
#define CSU_CTL_ENABLE_OFLOW_INTR_SHIFT     25
#define CSU_CTL_SELECT_CAPT_EVNT_SHIFT      28
#define CSU_CTL_ENABLE_SHIFT                31

#define CSU_CTL_ENABLE_MATCH_PARTID_BIT     (1 << CSU_CTL_ENABLE_MATCH_PARTID_SHIFT)
#define CSU_CTL_ENABLE_MATCH_PMG_BIT        (1 << CSU_CTL_ENABLE_MATCH_PMG_SHIFT)
#define CSU_CTL_ENABLE_OFLOW_FRZ_BIT        (1 << CSU_CTL_ENABLE_OFLOW_FRZ_SHIFT)
#define CSU_CTL_ENABLE_OFLOW_INTR_BIT       (1 << CSU_CTL_ENABLE_OFLOW_INTR_SHIFT)
#define CSU_CTL_ENABLE_BIT                  (1 << CSU_CTL_ENABLE_SHIFT)

/* MSMON_CFG_MBWU_FLT bit definitions */
#define MBWU_FLT_PARTID_SHIFT   0
#define MBWU_FLT_PMG_SHIFT      16

#define MBWU_FLT_PARTID_MASK    0xffff
#define MBWU_FLT_PMG_MASK       0xff

/* MSMON_CFG_MBWU_CTL bit definitions */
#define MBWU_CTL_TYPE_SHIFT          0
#define MBWU_CTL_OFLOW_STATUS_SHIFT  26

#define MBWU_CTL_TYPE_MASK           0xff
#define MBWU_CTL_OFLOW_STATUS_MASK   0x1

#define MBWU_CTL_ENABLE_MATCH_PARTID_SHIFT  16
#define MBWU_CTL_ENABLE_MATCH_PMG_SHIFT     17
#define MBWU_CTL_SELECT_SUBTYPE_SHIFT       20
#define MBWU_CTL_ENABLE_OFLOW_FRZ_SHIFT     24
#define MBWU_CTL_ENABLE_OFLOW_INTR_SHIFT    25
#define MBWU_CTL_ENABLE_CAPT_RESET_SHIFT    27
#define MBWU_CTL_SELECT_CAPT_EVNT_SHIFT     28
#define MBWU_CTL_ENABLE_SHIFT               31

#define MBWU_CTL_ENABLE_MATCH_PARTID_BIT    (1 << MBWU_CTL_ENABLE_MATCH_PARTID_SHIFT)
#define MBWU_CTL_ENABLE_MATCH_PMG_BIT       (1 << MBWU_CTL_ENABLE_MATCH_PMG_SHIFT)
#define MBWU_CTL_ENABLE_OFLOW_FRZ_BIT       (1 << MBWU_CTL_ENABLE_OFLOW_FRZ_SHIFT)
#define MBWU_CTL_ENABLE_OFLOW_INTR_BIT      (1 << MBWU_CTL_ENABLE_OFLOW_INTR_SHIFT)
#define MBWU_CTL_ENABLE_CAPT_RESET_BIT      (1 << MBWU_CTL_ENABLE_CAPT_RESET_SHIFT)
#define MBWU_CTL_ENABLE_BIT                 (1 << MBWU_CTL_ENABLE_SHIFT)

/* MSMON_CSU bit definitions */
#define CSU_VALUE_SHIFT     0
#define CSU_NRDY_SHIFT      31

#define CSU_VALUE_MASK      0x7fffffff
#define CSU_NRDY_MASK       0x1

/* MSMON_CSU_CAPTURE bit definitions */
#define CSU_CAPTURE_VALUE_SHIFT     0
#define CSU_CAPTURE_NRDY_SHIFT      31

#define CSU_CAPTURE_VALUE_MASK      0x7fffffff
#define CSU_CAPTURE_NRDY_MASK       0x1

/* MSMON_MBWU bit definitions */
#define MBWU_VALUE_SHIFT    0
#define MBWU_NRDY_SHIFT     31

#define MBWU_VALUE_MASK     0x7fffffff
#define MBWU_NRDY_MASK      0x1

/* MSMON_CSU_CAPTURE bit definitions */
#define MBWU_CAPTURE_VALUE_SHIFT    0
#define MBWU_CAPTURE_NRDY_SHIFT     31

#define MBWU_CAPTURE_VALUE_MASK     0x7fffffff
#define MBWU_CAPTURE_NRDY_MASK      0x1

/* MSMON_CAPT_EVNT bit definitions */
#define CAPT_EVNT_ENABLE_NOW_SHIFT  0
#define CAPT_EVNT_ENABLE_ALL_SHIFT  1

#define CAPT_EVNT_ENABLE_NOW_BIT    (1 << CAPT_EVNT_ENABLE_NOW_SHIFT)
#define CAPT_EVNT_ENABLE_ALL_BIT    (1 << CAPT_EVNT_ENABLE_ALL_SHIFT)

/*******************************************************************************
 * MPAM memory mapped error control and status register offsets & constants
 ******************************************************************************/
#define REG_MPAMF_ECR               0x00F0
#define REG_MPAMF_ESR               0x00F8

/* MPAMF_ECR bit definitions */
#define ECR_ENABLE_INTEN_SHIFT      0
#define ECR_ENABLE_INTEN_BIT        (1 << ECR_ENABLE_INTEN_SHIFT)

/* MPAMF_ESR bit definitions */
#define ESR_PARTID_MON_SHIFT        0
#define ESR_PMG_SHIFT               16
#define ESR_ERRCODE_SHIFT           24
#define ESR_OVRWR_SHIFT             31

#define ESR_PARTID_MON_MASK         0xffff
#define ESR_PMG_MASK                0xff
#define ESR_ERRCODE_MASK            0xf
#define ESR_OVRWR_MASK              0x1

/*******************************************************************************
 * MPAM error conditions in accessing memory mapped registers
 ******************************************************************************/
#define MPAM_ERRCODE_NO_ERROR               0
#define MPAM_ERRCODE_PARTID_SEL_RANGE       1
#define MPAM_ERRCODE_REQ_PARTID_RANGE       2
#define MPAM_ERRCODE_MSMONCFG_ID_RANGE      3
#define MPAM_ERRCODE_REQ_PMG_RANGE          4
#define MPAM_ERRCODE_MONITOR_RANGE          5
#define MPAM_ERRCODE_INTPARTID_RANGE        6
#define MPAM_ERRCODE_UNEXPECTED_INTERNAL    7
#define MPAM_ERRCODE_RESERVED               8

#endif
