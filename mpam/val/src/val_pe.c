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

#include "include/val_infra.h"
#include "include/val_pe.h"
#include "include/val_std_smc.h"


/* Pointer to the memory location of the PE Information table */
extern PE_INFO_TABLE *g_pe_info_table;

/* Global structure to pass and retrieve arguments for the SMC call */
extern ARM_SMC_ARGS g_smc_args;


/**
 * @brief   This API provides a 'C' interface to call System register reads
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   reg_id  - the system register index for which data is returned
 * @return  the value read from the system register.
 */
uint64_t val_pe_reg_read(uint32_t reg_id)
{

    switch (reg_id) {
        case MPIDR_EL1:
            return arm64_read_mpidr();
        case ID_AA64PFR0_EL1:
            return arm64_read_idpfr0();
        case ID_AA64PFR1_EL1:
            return arm64_read_idpfr1();
        case ID_AA64MMFR0_EL1:
            return arm64_read_mmfr0();
        case ID_AA64MMFR1_EL1:
            return arm64_read_mmfr1();
        case ID_AA64MMFR2_EL1:
            return arm64_read_mmfr2();
        case CTR_EL0:
            return arm64_read_ctr();
        case ID_AA64ISAR0_EL1:
            return arm64_read_isar0();
        case ID_AA64ISAR1_EL1:
            return arm64_read_isar1();
        case SCTLR_EL3:
            return arm64_read_sctlr3();
        case SCTLR_EL2:
            return arm64_read_sctlr2();
        case ID_AA64DFR0_EL1:
            return arm64_read_iddfr0();
        case ID_AA64DFR1_EL1:
            return arm64_read_iddfr1();
        case CurrentEL:
            return arm64_read_cur_el();
        case MDCR_EL2:
            return arm64_read_mdcr2();
        case VBAR_EL2:
            return arm64_read_vbar2();
        case CCSIDR_EL1:
            return arm64_read_ccsidr();
        case CSSELR_EL1:
            return arm64_read_csselr();
        case CLIDR_EL1:
            return arm64_read_clidr();
        case ID_DFR0_EL1:
            return arm_read_dfr0();
        case ID_ISAR0_EL1:
            return arm_read_isar0();
        case ID_ISAR1_EL1:
            return arm_read_isar1();
        case ID_ISAR2_EL1:
            return arm_read_isar2();
        case ID_ISAR3_EL1:
            return arm_read_isar3();
        case ID_ISAR4_EL1:
            return arm_read_isar4();
        case ID_ISAR5_EL1:
            return arm_read_isar5();
        case MIDR_EL1:
            return arm_read_midr();
        case MVFR0_EL1:
            return arm_read_mvfr0();
        case MVFR1_EL1:
            return arm_read_mvfr1();
        case MVFR2_EL1:
            return arm_read_mvfr2();
        case PMCEID0_EL0:
            return arm64_read_pmceid0();
        case PMCEID1_EL0:
            return arm64_read_pmceid1();
        case VMPIDR_EL2:
            return arm64_read_vmpidr();
        case VPIDR_EL2:
            return arm64_read_vpidr();
        case PMBIDR_EL1:
            return arm64_read_pmbidr();
        case PMSIDR_EL1:
            return arm64_read_pmsidr();
        case LORID_EL1:
            return arm64_read_lorid();
        case ERRIDR_EL1:
            return arm64_read_erridr();
        case ERR0FR_EL1:
            return arm64_read_err0fr();
        case ERR1FR_EL1:
            return arm64_read_err1fr();
        case ERR2FR_EL1:
            return arm64_read_err2fr();
        case ERR3FR_EL1:
            return arm64_read_err3fr();
        case ESR_EL2:
            return arm64_read_esr2();
        case FAR_EL2:
            return arm64_read_far2();
        default:
            val_report_status(val_pe_get_index_mpid(val_pe_get_mpid()), RESULT_FAIL(0, 0x78));
  }

  return 0x0;
}

/**
 * @brief   This API provides a 'C' interface to call System register writes
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   reg_id  - the system register index for which data is written
 * @param   write_data - the 64-bit data to write to the system register
 * @return  None
 */
void val_pe_reg_write(uint32_t reg_id, uint64_t write_data)
{

    switch (reg_id) {
        case CSSELR_EL1:
            arm64_write_csselr(write_data);
            break;
        case PMOVSSET_EL0:
            arm64_write_pmovsset(write_data);
            break;
        case PMOVSCLR_EL0:
            arm64_write_pmovsclr(write_data);
            break;
        case PMINTENSET_EL1:
            arm64_write_pmintenset(write_data);
            break;
        case PMINTENCLR_EL1:
            arm64_write_pmintenclr(write_data);
            break;
        case MDCR_EL2:
            arm64_write_mdcr2(write_data);
            break;
        case VBAR_EL2:
            arm64_write_vbar2(write_data);
            break;
        case PMSIRR_EL1:
            arm64_write_pmsirr(write_data);
            break;
        case PMSCR_EL2:
            arm64_write_pmscr2(write_data);
            break;
        case PMSFCR_EL1:
            arm64_write_pmsfcr(write_data);
            break;
        case PMBPTR_EL1:
            arm64_write_pmbptr(write_data);
            break;
        case PMBLIMITR_EL1:
            arm64_write_pmblimitr(write_data);
            break;
        default:
            val_report_status(val_pe_get_index_mpid(val_pe_get_mpid()), RESULT_FAIL(0, 0x78));
        }
}

/**
 * @brief   This API indicates the presence of exception level 3
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   None
 * @return  1 if EL3 is present, 0 if EL3 is not implemented
*/
uint8_t val_is_el3_enabled()
{

    uint64_t data;
    data = val_pe_reg_read(ID_AA64PFR0_EL1);
    return ((data >> 12) & 0xF);
}

/**
 * @brief   This API indicates the presence of exception level 2
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   None
 * @return  1 if EL2 is present, 0 if EL2 is not implemented
 */
uint8_t val_is_el2_enabled()
{

    uint64_t data;
    data = val_pe_reg_read(ID_AA64PFR0_EL1);
    return ((data >> 8) & 0xF);
}

/**
 * @brief   This API returns the PMU Overflow Signal Interrupt ID for a given PE index
 *          1. Caller       -  Test Suite, VAL
 *          2. Prerequisite -  val_create_peinfo_table
 * @param   index - the index of PE whose PMU interrupt ID is returned.
 * @return  PMU interrupt id
 */
uint32_t val_pe_get_pmu_gsiv(uint32_t index)
{

    PE_INFO_ENTRY *entry;

    if (index > g_pe_info_table->header.num_of_pe) {
        val_report_status(index, RESULT_FAIL(0, 0xFF));
        return 0xFFFFFF;
    }

    entry = g_pe_info_table->pe_info;

    return entry[index].pmu_gsiv;
}
