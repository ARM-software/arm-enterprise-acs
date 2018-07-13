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

#include <Library/UefiLib.h>

#include "include/pal_pmu.h"
#include "include/pal_uefi.h"

#define PMCCFILTR_NSH_EN_BIT    27
#define PMCNTENSET_C_EN_BIT     31
#define PMCR_LC_EN_BIT          6
#define PMCR_C_RESET_BIT        2
#define PMCR_EN_BIT             0

STATIC UINT64 ControlRegBackup;
STATIC UINT64 CcFiltRegBackup;

/**
 * @brief   This API provides PAL interface to PMU register reads
 *
 * @param   RegId   - the pmu register index for which data is returned
 * @return  the value read from the pmu register.
 */
UINT64
pal_pmu_reg_read (
  UINT32 RegId
  )
{

    switch (RegId) {
        case PMCR_EL0:
            return Arm64ReadPmcr();
        case PMCCNTR_EL0:
            return Arm64ReadPmccntr();
        case PMCCFILTR_EL0:
            return Arm64ReadPmccfiltr();
        case PMCNTENSET_EL0:
            return Arm64ReadPmcntenset();
        default:
            acs_print(ACS_PRINT_ERR, L"\n FATAL - Unsupported PMU register read \n");
    }

    return 0x0;
}

/**
 * @brief   This API provides PAL interface to PMU register writes
 *
 * @param   reg_id  - the pmu register index for which data is written
 * @param   write_data - the 64-bit data to write to the pmu register
 * @return  None
 */
VOID
pal_pmu_reg_write (
  UINT32 RegId,
  UINT64 WriteData
  )
{

    switch (RegId) {
        case PMCR_EL0:
            Arm64WritePmcr(WriteData);
            break;
        case PMCCNTR_EL0:
            Arm64WritePmccntr(WriteData);
            break;
        case PMCCFILTR_EL0:
            Arm64WritePmccfiltr(WriteData);
            break;
        case PMCNTENSET_EL0:
            Arm64WritePmcntenset(WriteData);
            break;
        default:
            acs_print(ACS_PRINT_ERR, L"\n FATAL - Unsupported PMU register read \n");
    }
}

/**
 * @brief   Configures necessary PMU registers & starts the Cycle Counter
 *
 * @param   None
 * @return  None
 */
VOID
pal_pmu_cycle_counter_start (
  )
{

    UINT64 ControlReg;
    UINT64 CcFiltReg;
    UINT64 CcEnableSetReg;

    /* Save PMU Filter Register settings to a temp storage */
    CcFiltReg = pal_pmu_reg_read(PMCCFILTR_EL0);
    CcFiltRegBackup = CcFiltReg;

    /* Save PMU Control Register settings to a temp storage */
    ControlReg = pal_pmu_reg_read(PMCR_EL0);
    ControlRegBackup = ControlReg;

    /* Enable the PMU filter to count cycles in EL2 mode */
    CcFiltReg |= (1 << PMCCFILTR_NSH_EN_BIT);
    pal_pmu_reg_write(PMCCFILTR_EL0, CcFiltReg);
    CcFiltReg = pal_pmu_reg_read(PMCCFILTR_EL0);

    /* Enable the PMU Cycle Count Register */
    CcEnableSetReg = pal_pmu_reg_read(PMCNTENSET_EL0);
    CcEnableSetReg |= (1 << PMCNTENSET_C_EN_BIT);
    pal_pmu_reg_write(PMCNTENSET_EL0, CcEnableSetReg);
    CcEnableSetReg = pal_pmu_reg_read(PMCNTENSET_EL0);

    /* Enable the PMU Long Cycle Counter, reset the Cycle Counter and set Global Enable */
    ControlReg |= (1 << PMCR_LC_EN_BIT) | (1 << PMCR_C_RESET_BIT) | (1 << PMCR_EN_BIT);
    pal_pmu_reg_write(PMCR_EL0, ControlReg);
    ControlReg = pal_pmu_reg_read(PMCR_EL0);

}

/**
 * @brief   Disables the Cycle Counter and restores the PMU reg values
 *
 * @param   None
 * @return  None
 */
VOID
pal_pmu_cycle_counter_stop (
  )
{

    UINT64 ControlReg;
    UINT64 CcFiltReg;

    /* Disable Cycle Counter & restore PMU Control Reg settings */
    ControlReg = ControlRegBackup;
    pal_pmu_reg_write(PMCR_EL0, ControlReg);

    /* Restore PMU Filter Register settings */
    CcFiltReg = CcFiltRegBackup;
    pal_pmu_reg_write(PMCCFILTR_EL0, CcFiltReg);
}
