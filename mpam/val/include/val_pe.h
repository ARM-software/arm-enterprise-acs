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

#ifndef __MPAM_ACS_PE_H__
#define __MPAM_ACS_PE_H__

#define MAX_NUM_PE_LEVEL0   0x8
#define MAX_NUM_PE_LEVEL2   (2 << 27)

/* AARCH64 processor exception types */
#define EXCEPT_AARCH64_SYNC     0
#define EXCEPT_AARCH64_IRQ      1
#define EXCEPT_AARCH64_FIQ      2
#define EXCEPT_AARCH64_SERROR   3

typedef enum {
    MPIDR_EL1 = 1,
    ID_AA64PFR0_EL1,
    ID_AA64PFR1_EL1,
    ID_AA64MMFR0_EL1,
    ID_AA64MMFR1_EL1,
    ID_AA64MMFR2_EL1,
    ID_AA64DFR0_EL1,
    ID_AA64DFR1_EL1,
    CTR_EL0,
    ID_AA64ISAR0_EL1,
    ID_AA64ISAR1_EL1,
    SCTLR_EL3,
    SCTLR_EL2,
    PMOVSSET_EL0,
    PMOVSCLR_EL0,
    PMINTENSET_EL1,
    PMINTENCLR_EL1,
    CurrentEL,
    MDCR_EL2,
    VBAR_EL2,
    CCSIDR_EL1,
    CSSELR_EL1,
    CLIDR_EL1,
    ID_DFR0_EL1,
    ID_ISAR0_EL1,
    ID_ISAR1_EL1,
    ID_ISAR2_EL1,
    ID_ISAR3_EL1,
    ID_ISAR4_EL1,
    ID_ISAR5_EL1,
    MIDR_EL1,
    MVFR0_EL1,
    MVFR1_EL1,
    MVFR2_EL1,
    PMCEID0_EL0,
    PMCEID1_EL0,
    VMPIDR_EL2,
    VPIDR_EL2,
    PMBIDR_EL1,
    PMSIDR_EL1,
    LORID_EL1,
    ERRIDR_EL1,
    ERR0FR_EL1,
    ERR1FR_EL1,
    ERR2FR_EL1,
    ERR3FR_EL1,
    PMSIRR_EL1,
    PMSCR_EL2,
    PMSFCR_EL1,
    PMBPTR_EL1,
    PMBLIMITR_EL1,
    ESR_EL2,
    FAR_EL2,
} MPAM_ACS_PE_REGS;


uint64_t arm64_read_mpidr(void);

uint64_t arm64_read_idpfr0(void);

uint64_t arm64_read_idpfr1(void);

uint64_t arm64_read_mmfr0(void);

uint64_t arm64_read_mmfr1(void);

uint64_t arm64_read_mmfr2(void);

uint64_t arm64_read_ctr(void);

uint64_t arm64_read_isar0(void);

uint64_t arm64_read_isar1(void);

uint64_t arm64_read_sctlr3(void);

uint64_t arm64_read_sctlr2(void);

uint64_t arm64_read_pmcr(void);

uint64_t arm64_read_iddfr0(void);

uint64_t arm64_read_iddfr1(void);

uint64_t arm64_read_cur_el(void);

uint64_t arm64_read_mdcr2(void);

void arm64_write_mdcr2(uint64_t write_data);

uint64_t arm64_read_vbar2(void);

void arm64_write_vbar2(uint64_t write_data);

void arm64_write_pmcr(uint64_t write_data);

void arm64_write_pmovsset(uint64_t write_data);

void arm64_write_pmovsclr(uint64_t write_data);

void arm64_write_pmintenset(uint64_t write_data);

void arm64_write_pmintenclr(uint64_t write_data);

uint64_t arm64_read_ccsidr(void);

uint64_t arm64_read_csselr(void);

void arm64_write_csselr(uint64_t write_data);

uint64_t arm64_read_clidr(void);

uint64_t arm_read_dfr0(void);

uint64_t arm_read_isar0(void);

uint64_t arm_read_isar1(void);

uint64_t arm_read_isar2(void);

uint64_t arm_read_isar3(void);

uint64_t arm_read_isar4(void);

uint64_t arm_read_isar5(void);

uint64_t arm_read_mmfr0(void);

uint64_t arm_read_mmfr1(void);

uint64_t arm_read_mmfr2(void);

uint64_t arm_read_mmfr3(void);

uint64_t arm_read_mmfr4(void);

uint64_t arm_read_pfr0(void);

uint64_t arm_read_pfr1(void);

uint64_t arm_read_midr(void);

uint64_t arm_read_mvfr0(void);

uint64_t arm_read_mvfr1(void);

uint64_t arm_read_mvfr2(void);

uint64_t arm64_read_pmceid0(void);

uint64_t arm64_read_pmceid1(void);

uint64_t arm64_read_vmpidr(void);

uint64_t arm64_read_vpidr(void);

uint64_t arm64_read_pmbidr(void);

uint64_t arm64_read_pmsidr(void);

uint64_t arm64_read_lorid(void);

uint64_t arm64_read_erridr(void);

uint64_t arm64_read_err0fr(void);

uint64_t arm64_read_err1fr(void);

uint64_t arm64_read_err2fr(void);

uint64_t arm64_read_err3fr(void);

void arm64_write_pmsirr(uint64_t write_data);

void arm64_write_pmscr2(uint64_t write_data);

void arm64_write_pmsfcr(uint64_t write_data);

void arm64_write_pmbptr(uint64_t write_data);

void arm64_write_pmblimitr(uint64_t write_data);

uint64_t arm64_read_esr2(void);

uint64_t arm64_read_sp(void);

uint64_t arm64_write_sp(uint64_t write_data);

uint64_t arm64_read_far2(void);

uint64_t val_pe_reg_read(uint32_t reg_id);
void val_pe_reg_write(uint32_t reg_id, uint64_t write_data);
void val_pe_update_elr(void *context, uint64_t offset);
uint64_t val_pe_get_esr(void *context);
uint64_t val_pe_get_far(void *context);
void val_pe_context_save(uint64_t sp, uint64_t elr);
void val_pe_initialize_default_exception_handler(void (*esr)(uint64_t, void *));
void val_pe_context_restore(uint64_t sp);
void val_pe_default_esr(uint64_t interrupt_type, void *context);
void val_pe_cache_clean_range(uint64_t start_addr, uint64_t length);

uint32_t c001_entry(void);
uint32_t c002_entry(uint32_t num_pe);

#endif

