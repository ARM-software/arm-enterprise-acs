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

#ifndef __MPAM_ACS_PMU_H__
#define __MPAM_ACS_PMU_H__

/* EL2 Cycle Count Filter Enable */
#define NSH_EN    27
/* Cycle Counter Enable */
#define CC_EN     31
/* Long Cycle Counter Enable */
#define LCC_EN    06
/* Cycle Count Reset */
#define CCR_SET   02
/* Global Cycle Counter enable */
#define GCC_EN    00

typedef enum {
    PMCR_EL0 = 1,
    PMCCNTR_EL0,
    PMCCFILTR_EL0,
    PMCNTENSET_EL0
} PAL_PMU_REGS;

UINT64 pal_pmu_reg_read(UINT32 RegId);
VOID pal_pmu_reg_write(UINT32 RegId, UINT64 WriteData);
VOID pal_pmu_cycle_counter_start();
VOID pal_pmu_cycle_counter_stop();

UINT64 Arm64ReadPmccntr(VOID);
UINT64 Arm64ReadPmcr(VOID);
UINT64 Arm64ReadPmccfiltr(VOID);
UINT64 Arm64ReadPmcntenset(VOID);
VOID Arm64WritePmcr(UINT64 WriteData);
VOID Arm64WritePmccntr(UINT64 WriteData);
VOID Arm64WritePmccfiltr(UINT64 WriteData);
VOID Arm64WritePmcntenset(UINT64 WriteData);

#endif

