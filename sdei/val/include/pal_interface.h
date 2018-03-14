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

#ifndef __PAL_INTERFACE_H
#define __PAL_INTERFACE_H

#ifdef TARGET_LINUX
  #include <linux/module.h>
  typedef uint64_t addr_t;
#else
  typedef CHAR8  char8_t;
  typedef CHAR16 char16_t;
  typedef UINT8  uint8_t;
  typedef UINT16 uint16_t;
  typedef UINT32 uint32_t;
  typedef UINT32 size_t;
  typedef UINT64 uint64_t;
  typedef INT8  int8_t;
  typedef INT16 int16_t;
  typedef INT32 int32_t;
  typedef INT64 int64_t;
  typedef BOOLEAN bool;
  typedef UINT64 addr_t;
#endif

enum sdei_status {
    SDEI_STATUS_SUCCESS = 0,
    SDEI_STATUS_NOT_SUPPORTED = 0x80000000,
    SDEI_STATUS_INVALID,
    SDEI_STATUS_DENIED,
    SDEI_STATUS_PENDING,
    SDEI_STATUS_OUT_OF_RESOURCE,
};

#define SDEI_EVENT_TYPE_PRIVATE            0
#define SDEI_EVENT_TYPE_SHARED             1
#define SDEI_EVENT_TYPE_ANY                2
#define SDEI_EVENT_PRIORITY_NORMAL         0
#define SDEI_EVENT_PRIORITY_CRITICAL       1
#define SDEI_EVENT_PRIORITY_ANY            2
#define SDEI_EVENT_UNBOUND                 0x10000

#define WD_REG_CTRL 0x8
#define WD_REG_BASE 0x0

#define WD_ENABLE      0x1
#define WD_DISABLE     0x0

typedef struct {
    uint32_t number;
} event_info_t;

typedef struct {
    uint32_t num_events;
    int32_t  hest_found;
    uint32_t num_ghes_notify;
    event_info_t info[];
} event_info_table_t;

int pal_invoke_sdei_fn(unsigned long function_id, unsigned long arg0,
            unsigned long arg1, unsigned long arg2,
            unsigned long arg3, unsigned long arg4,
            uint64_t *result);

int pal_sdei_create_event_info_table(event_info_table_t *event_table);
void *pal_intf_alloc(int size);
void pal_intf_free(void *handle);
unsigned int pal_smp_pe_id(void);
void pal_intf_lock(void);
void pal_intf_unlock(void);
void pal_interface_broken(void);
int pal_conduit_get(void);
int pal_acpi_present(void);
void pal_print_raw(char *string, uint64_t data);

void pal_print(uint32_t verbosity, char *str, ...);

typedef struct sdei_log_control {
    int print_level;
    void *log_file_handle;
} sdei_log_control;

typedef struct {
    uint64_t data0;
    uint64_t data1;
    uint32_t status;
} pe_shared_mem_t;

/* PE Test related Definitions */

/**
 * @brief  number of PEs discovered
**/
typedef struct {
  uint32_t num_of_pe;
} pe_info_hdr_t;

/**
 * @brief  structure instance for PE entry
**/
typedef struct {
  uint32_t   pe_num;    ///< PE Index
  uint32_t   attr;      ///< PE attributes
  uint64_t   mpidr;     ///< PE MPIDR
  uint32_t   pmu_gsiv;  ///< PMU Interrupt ID
} pe_info_entry_t;

typedef struct {
  pe_info_hdr_t    header;
  pe_info_entry_t  pe_info[];
} pe_info_table_t;

typedef enum {
    ACS_LOG_ERR = 1,
    ACS_LOG_WARN,
    ACS_LOG_TEST,
    ACS_LOG_DEBUG,
    ACS_LOG_INFO,
    ACS_LOG_KERNEL
} acs_log_level_t;

typedef enum {
    ACS_SUCCESS,
    ACS_ERROR
} acs_status_t;

void pal_pe_create_info_table(pe_info_table_t *pe_info_table_t);
int pal_pe_execute_on_all(int num_pe, void *payload, uint64_t arg);
void pal_pe_suspend(uint32_t power_state);
void pal_pe_poweroff(uint32_t pe_index);
void pal_pe_poweron(uint64_t pe_mpidr);
acs_status_t pal_pe_alloc_shared_mem(uint32_t num_pe, size_t size);
void pal_pe_free_shared_mem(void);
void pal_pe_clean_up(void);
acs_status_t pal_pe_read_shared_mem(uint32_t index, pe_shared_mem_t *dst);
acs_status_t pal_pe_write_shared_mem(uint32_t index, pe_shared_mem_t *src);
void pal_pe_data_cache_invalidate(uint64_t);
void pal_pe_data_cache_clean_invalidate(uint64_t);

/**
 * @brief  GIC Info header - Summary of GIC subsytem
**/
typedef struct {
  uint32_t gic_version;
  uint32_t num_gicd;
  uint32_t num_gicrd;
  uint32_t num_its;
} gic_info_hdr_t;

typedef enum {
  ENTRY_TYPE_CPUIF = 0x1000,
  ENTRY_TYPE_GICD,
  ENTRY_TYPE_GICRD,
  ENTRY_TYPE_GICITS
} gic_info_type_t;

/**
 * @brief  structure instance for GIC entry
**/
typedef struct {
  uint32_t type;
  uint64_t base;
} gic_info_entry_t;

/**
 * @brief  GIC Information Table
**/
typedef struct {
  gic_info_hdr_t   header;
  gic_info_entry_t gic_info[];  ///< Array of Information blocks - instantiated for each GIC type
} gic_info_table_t;

void     pal_gic_create_info_table(gic_info_table_t *gic_info_table);
uint32_t pal_gic_install_isr(uint32_t int_id, void *isr);
void     pal_gic_end_of_interrupt(uint32_t int_id);
uint32_t pal_gic_free_interrupt(uint32_t int_id);

/**
 * @brief  Watchdog Info header - Summary of Watchdog subsytem
**/
typedef struct {
  uint32_t num_wd;  ///< number of Watchdogs present in the system
} wd_info_hdr_t;

/**
 * @brief  structure instance for Watchdog entry
**/
typedef struct {
  uint64_t wd_ctrl_base;     ///< Watchdog Control Register Frame
  uint64_t wd_refresh_base;  ///< Watchdog Refresh Register Frame
  uint32_t wd_gsiv;          ///< Watchdog Interrupt ID
  uint32_t wd_flags;
} wd_info_entry_t;

/**
 * @brief WD Information Table
**/
typedef struct {
    wd_info_hdr_t header;
    wd_info_entry_t wd_info[];
} wd_info_table_t;

uint32_t pal_wd_create_info_table(wd_info_table_t *wd_info_table);

/* Timer tests related definitions */

/**
 * @brief  Timer Info header - Summary of Timer subsytem
**/
typedef struct {
  uint32_t s_el1_timer_flag;
  uint32_t ns_el1_timer_flag;
  uint32_t el2_timer_flag;
  uint32_t el2_virt_timer_flag;
  uint32_t s_el1_timer_gsiv;
  uint32_t ns_el1_timer_gsiv;
  uint32_t el2_timer_gsiv;
  uint32_t virtual_timer_flag;
  uint32_t virtual_timer_gsiv;
  uint32_t el2_virt_timer_gsiv;
  uint32_t num_platform_timer;
  uint32_t num_watchdog;
  uint32_t sys_timer_status;
} timer_info_hdr_t;

#define TIMER_TYPE_SYS_TIMER 0x2001

/**
 * @brief  structure instance for TIMER entry
**/
typedef struct {
  uint32_t type;
  uint32_t timer_count;
  uint64_t block_cntl_base;
  uint8_t  frame_num[8];
  uint64_t GtCntBase[8];
  uint64_t GtCntEl0Base[8];
  uint32_t gsiv[8];
  uint32_t virt_gsiv[8];
  uint32_t flags[8];
} timer_info_gtblock_t;

typedef struct {
  timer_info_hdr_t     header;
  timer_info_gtblock_t gt_info[];
} timer_info_table_t;

void pal_timer_create_info_table(timer_info_table_t *timer_info_table);

void pal_wd_set_ws0(uint64_t *vaddr, uint64_t base, uint32_t index, uint32_t timeout);
acs_status_t pal_configure_second_interrupt(uint32_t *index, uint64_t *int_id);
void pal_generate_second_interrupt(uint32_t index, uint32_t timeout);
void pal_disable_second_interrupt(uint32_t index);

uint32_t pal_mmio_read(uint64_t addr);
void pal_mmio_write(uint64_t addr, uint32_t data);
uint64_t *pal_pa_to_va(uint64_t addr);
void pal_va_write(uint64_t *addr, uint32_t offset, uint32_t data);
void pal_va_free(uint64_t *addr);
void pal_free_mem(uint64_t *ptr);

int32_t pal_invoke_psci_fn(uint64_t function_id, uint64_t arg0,
                                    uint64_t arg1, uint64_t arg2);
#endif
