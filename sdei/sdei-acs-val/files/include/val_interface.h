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

#ifndef __VAL_INTERFACE_H
#define __VAL_INTERFACE_H

#include "pal_interface.h"
#include "val_test_infra.h"
#ifdef TARGET_LINUX
  #define TRUE 1
  #define FALSE 0
  #define CLIENT_EL 0x1
#else
  #define CLIENT_EL 0x2
#endif
#include "val_pe.h"
#define val_print(verbosity, fmt, ...) pal_print(verbosity, fmt, ##__VA_ARGS__)
#define val_print_raw(string, data) pal_print_raw(string, data)
#define GIC_INFO_VERSION 3
#define EVENT_STATUS_REGISTER_BIT (1 << 0)
#define EVENT_STATUS_ENABLE_BIT   (1 << 1)
#define EVENT_STATUS_RUNNING_BIT  (1 << 2)
#define __EXTRACT_BITS(data, start, length) ((data >> start) & ((1ul << length)-1))

#define PE_MPIDR_LEVEL_BITS 8
#define PE_MPIDR_LEVEL_MASK ((1 << PE_MPIDR_LEVEL_BITS) - 1)
#define PE_MPIDR_AFFINITY_LEVEL(mpidr, level) \
                 ((mpidr >> (PE_MPIDR_LEVEL_BITS * level)) & PE_MPIDR_LEVEL_MASK)
#define INVALID_INDEX 0xFFFFFFFF

/* Below interrupt numbers used in SDEI testing */
#define SPI_INTR_NUM  230
#define SPI_INTR_NUM1 231
#define PPI_INTR_NUM  18
#define SGI_INTR_NUM  5

extern pe_shared_mem_t *g_pe_shared_mem;

typedef enum {
  MPIDR_EL1 = 1,
  ID_AA64PFR0_EL1,
  CurrentEL,
  DAIF,
  SPsel,
  ELR_EL,
  SPSR_EL,
} pe_reg_id_t;

int32_t val_sdei_initialization(void);

acs_status_t
val_sdei_create_event_info_table(uint64_t *event_info_table);
void val_event_free_info_table(void);

uint32_t
val_pe_get_index(void);

uint32_t
val_pe_get_num(void);

acs_status_t
val_pe_execute_on_all(void *payload, uint64_t arg);

acs_status_t
val_pe_create_info_table(void *);

void
val_pe_data_cache_clean_invalidate(uint64_t addr);

void
val_pe_data_cache_invalidate(uint64_t addr);

void
val_pe_suspend(uint32_t power_state);

void val_pe_free_info_table(void);
void
val_pe_poweroff(uint32_t pe_index);

void
val_pe_poweron(uint32_t pe_index);

acs_status_t
val_shared_mem_alloc(void);

acs_status_t
val_shared_mem_read(uint32_t index, pe_shared_mem_t *dst);

acs_status_t
val_shared_mem_write(uint32_t index, pe_shared_mem_t *src);

void
val_shared_mem_free(void);

acs_status_t
val_gic_create_info_table(uint64_t *gic_info_table);

acs_status_t
val_pe_reg_read(pe_reg_id_t reg_id, uint64_t *val);

void
val_gic_free_info_table(void);

acs_status_t
val_get_gicd_base(uint64_t *gicd_base);

acs_status_t
val_gic_get_version(uint32_t *version);

acs_status_t val_gic_install_isr(uint32_t int_id, void *isr);

acs_status_t val_gic_end_of_interrupt(uint32_t int_id);

acs_status_t val_gic_route_interrupt_to_pe(uint32_t int_id, uint64_t mpidr);

acs_status_t val_gic_get_interrupt_state(uint32_t int_id, uint32_t *status);

acs_status_t val_gic_clear_interrupt(uint32_t int_id);

acs_status_t val_gic_disable_interrupt(uint32_t int_id);
acs_status_t val_gic_generate_interrupt(uint32_t int_id);
acs_status_t val_gic_free_interrupt(uint32_t int_id);
uint64_t val_gic_mpidr_to_affinity(uint64_t mpidr);

/* Watchdog VAL APIs */
typedef enum {
  WD_INFO_COUNT = 1,
  WD_INFO_CTRL_BASE,
  WD_INFO_REFRESH_BASE,
  WD_INFO_GSIV,
  WD_INFO_ISSECURE
} WD_INFO_TYPE;

#define WD_TIME_OUT      0x10000000
#define TIMEOUT_MEDIUM    0x100000
int32_t val_wd_create_info_table(uint64_t *wd_info_table);
uint64_t val_wd_get_info(uint32_t index, uint32_t info_type);
void val_wd_enable(uint32_t index);
void val_wd_disable(uint32_t index);
void val_wd_set_ws0(uint64_t *vaddr, uint32_t index, uint32_t timeout);
void val_watchdog_free_info_table(void);

acs_status_t
val_gic_cpuif_init(void);

uint32_t
val_mmio_read(uint64_t addr);

void
val_mmio_write(uint64_t addr, uint32_t data);

uint64_t *val_pa_to_va(uint64_t addr);
void val_va_write(uint64_t *addr, uint32_t offset, uint32_t data);
void val_va_free(uint64_t *addr);
void val_intf_lock(void);
void val_intf_unlock(void);

void asm_event_handler(void);
void asm_handler_resume(void);
void asm_handler_resume_context(void);

/*TIMER VAL APIs */
typedef enum {
  TIMER_INFO_CNTFREQ = 1,
  TIMER_INFO_PHY_EL1_INTID,
  TIMER_INFO_PHY_EL1_FLAGS,
  TIMER_INFO_VIR_EL1_INTID,
  TIMER_INFO_VIR_EL1_FLAGS,
  TIMER_INFO_PHY_EL2_INTID,
  TIMER_INFO_PHY_EL2_FLAGS,
  TIMER_INFO_VIR_EL2_INTID,
  TIMER_INFO_VIR_EL2_FLAGS,
  TIMER_INFO_NUM_PLATFORM_TIMERS,
  TIMER_INFO_IS_PLATFORM_TIMER_SECURE,
  TIMER_INFO_SYS_CNTL_BASE,
  TIMER_INFO_SYS_CNT_BASE_N,
  TIMER_INFO_SYS_INTID,
  TIMER_INFO_SYS_TIMER_STATUS
} timer_info_t;

#define SDEI_TIMER_FLAG_ALWAYS_ON 0x4

void     val_timer_create_info_table(uint64_t *timer_info_table);
void     val_timer_free_info_table(void);
uint32_t val_timer_execute_tests(uint32_t level, uint32_t num_pe);
uint64_t val_timer_get_info(timer_info_t info_type, uint64_t instance);
void     val_timer_set_phy_el1(uint64_t timeout);
void     val_timer_set_vir_el1(uint64_t timeout);
void     val_timer_set_phy_el2(uint64_t timeout);
void     val_timer_set_vir_el2(uint64_t timeout);
void     val_timer_set_system_timer(addr_t cnt_base_n, uint32_t timeout);
void     val_timer_disable_system_timer(addr_t cnt_base_n);
void     val_timer_free_info_table(void);
uint32_t val_timer_skip_if_cntbase_access_not_allowed(uint64_t index);
void val_platform_timer_get_entry_index(uint64_t instance, uint32_t *block, uint32_t *index);
acs_status_t val_configure_second_interrupt(uint32_t *index, uint64_t *int_id);
void val_generate_second_interrupt(uint32_t index, uint32_t timeout);
void val_disable_second_interrupt(uint32_t index);

/* PSCI VAL APIs */
#define PSCI_FN_PSCI_VERSION        0x84000000
#define PSCI_FN_AFFINITY_INFO       0xC4000004
#define PSCI_FN_PSCI_FEATURES       0x8400000A
#define PSCI_FN_PSCI_CPU_SUSPEND    0xC400000E

uint32_t val_psci_version(uint32_t *version);
uint32_t val_psci_affinity_info(uint64_t target_aff, uint64_t lowest_aff_level);
uint32_t val_psci_features(uint32_t fun_id);
#endif
