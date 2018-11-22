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

#ifndef __VAL_INTERFACE_H__
#define __VAL_INTERFACE_H__

#include "pal_interface.h"

#define ACS_PRINT_ERR   5
#define ACS_PRINT_WARN  4
#define ACS_PRINT_TEST  3
#define ACS_PRINT_DEBUG 2
#define ACS_PRINT_INFO  1

#define ACS_STATUS_FAIL 0x90000000
#define ACS_STATUS_ERR  0xEDCB1234
#define ACS_STATUS_SKIP 0x10000000
#define ACS_STATUS_PASS 0x0
#define ACS_STATUS_EXIT 0xFFFFFFFF

#define GET_MAX_VALUE(ax, ay) (((ax) > (ay)) ? (ax) : (ay))
#define GET_MIN_VALUE(ax, ay) (((ax) > (ay)) ? (ay) : (ax))
/* The return value is always 64-bit, type casting is needed by the caller and (m > n) */
#define CLEAR_BITS_M_TO_N(num, m, n)  (num & ((~0UL << (m+1)) | ((1UL << n) - 1)))

/* GENERIC VAL APIs */
void val_allocate_shared_mem(void);
uint8_t val_allocate_shared_memcpybuf(uint64_t mem_base, uint64_t mem_size, uint64_t buf_size, uint32_t num_pe);
void val_allocate_shared_latencybuf(uint32_t node_cnt, uint32_t num_pe);
void val_free_shared_mem(void);
void *val_allocate_buf(uint64_t size);
void *val_allocate_address(uint64_t mem_base, uint64_t mem_size, uint64_t size);
void val_free_buf(void *buffer, uint64_t size);
void val_mem_copy(void *src, void *dest, uint64_t size);
void val_print(uint32_t level, char8_t *string, uint64_t data);
void val_print_raw(uint32_t level, char8_t *string, uint64_t data);
void val_set_test_data(uint32_t index, uint64_t addr, uint64_t test_data);
void val_get_test_data(uint32_t index, uint64_t *data0, uint64_t *data1);
uint64_t val_get_shared_memcpybuf(uint32_t pe_index);
void val_mem_free_shared_memcpybuf(uint32_t num_pe, uint64_t buf_size);
void val_mem_free_shared_latencybuf(uint32_t node_cnt);
uint64_t *val_get_shared_latencybuf(uint32_t scenario_index, uint32_t node_index);

/* VAL PE APIs */
uint32_t val_pe_create_info_table(uint64_t *pe_info_table);
void val_pe_free_info_table(void);
uint32_t val_pe_get_num(void);
uint64_t val_pe_get_mpid_index(uint32_t index);
uint32_t val_pe_get_pmu_gsiv(uint32_t index);
uint64_t val_pe_get_mpid(void);
uint32_t val_pe_get_index_mpid(uint64_t mpid);

void val_execute_on_pe(uint32_t index, void (*payload)(void), uint64_t args);
void val_suspend_pe(uint32_t power_state, uint64_t entry, uint32_t context_id);

/* GIC VAL APIs */
uint32_t    val_gic_create_info_table(uint64_t *gic_info_table);

/* MPAM VAL APIs */
uint32_t val_mpam_create_info_table();
uint32_t val_cache_execute_tests(uint32_t num_pe);
uint32_t val_csumon_execute_tests(uint32_t num_pe);
uint32_t val_interrupts_execute_tests(uint32_t num_pe);
uint32_t val_memory_execute_tests(uint32_t num_pe);

typedef enum {
    GIC_INFO_VERSION=1,
    GIC_INFO_SEC_STATES,
    GIC_INFO_DIST_BASE,
    GIC_INFO_CITF_BASE,
    GIC_INFO_NUM_RDIST,
    GIC_INFO_RDIST_BASE,
    GIC_INFO_NUM_ITS,
    GIC_INFO_ITS_BASE
} GIC_INFO_e;

uint32_t val_gic_get_info(GIC_INFO_e type);
void val_gic_free_info_table(void);
uint32_t val_gic_install_isr(uint32_t int_id, void (*isr)(void));
uint32_t val_gic_route_interrupt_to_pe(uint32_t int_id, uint64_t mpidr);
uint32_t val_gic_end_of_interrupt(uint32_t int_id);
void val_gic_write_ispendreg(uint32_t intr_id);

/* MEASUREMENTS VAL APIs */
void val_measurement_start();
void val_measurement_stop();
uint64_t val_measurement_read();

#endif
