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

#ifndef __PAL_INTERFACE_H__
#define __PAL_INTERFACE_H__

#define TIMEOUT_LARGE   0x1000000
#define TIMEOUT_MEDIUM  0x100000
#define TIMEOUT_SMALL   0x1000

#define CLEAN_AND_INVALIDATE    0x1
#define CLEAN                   0x2
#define INVALIDATE              0x3

typedef CHAR8    char8_t;
typedef CHAR16   char16_t;
typedef UINT8    uint8_t;
typedef UINT16   uint16_t;
typedef UINT32   uint32_t;
typedef UINT64   uint64_t;
typedef UINT64   addr_t;

typedef struct {
    uint64_t    Arg0;
    uint64_t    Arg1;
    uint64_t    Arg2;
    uint64_t    Arg3;
    uint64_t    Arg4;
    uint64_t    Arg5;
    uint64_t    Arg6;
    uint64_t    Arg7;
} ARM_SMC_ARGS;

/**
 * @brief  number of PEs discovered
 */
typedef struct {
    uint32_t    num_of_pe;
} PE_INFO_HDR;

/**
 * @brief  structure instance for PE entry
 */
typedef struct {
    uint32_t    pe_num;
    uint32_t    attr;
    uint64_t    mpidr;
    uint32_t    pmu_gsiv;
} PE_INFO_ENTRY;

typedef struct {
    PE_INFO_HDR     header;
    PE_INFO_ENTRY   pe_info[];
} PE_INFO_TABLE;


/**
 * @brief  GIC Info header - Summary of GIC subsystem
 */
typedef struct {
    uint32_t    gic_version;
    uint32_t    num_gicd;
    uint32_t    num_gicrd;
    uint32_t    num_its;
} GIC_INFO_HDR;

typedef enum {
    ENTRY_TYPE_CPUIF = 0x1000,
    ENTRY_TYPE_GICD,
    ENTRY_TYPE_GICRD,
    ENTRY_TYPE_GICITS
} GIC_INFO_TYPE_e;

/**
 * @brief  structure instance for GIC entry
 */
typedef struct {
    uint32_t    type;
    uint64_t    base;
} GIC_INFO_ENTRY;

/**
 * @brief  GIC Information Table
 */
typedef struct {
    GIC_INFO_HDR    header;
    GIC_INFO_ENTRY  gic_info[];
} GIC_INFO_TABLE;


typedef enum {
    ALLOC_TYPE_RD = 0x0,
    ALLOC_TYPE_WR,
    ALLOC_TYPE_RW,
} ALLOC_TYPE;

typedef enum {
    CACHE_TYPE_DATA = 0x0,
    CAHE_TYPE_INST,
    CACHE_TYPE_UNIFIED,
} CACHE_TYPE;

typedef enum {
    WRITE_POLICY_WB = 0x0,
    WRITE_POLICY_WT
} WRITE_POLICY;

/*
 * @brief   Cache Type Structure Cache Attributes
 */
typedef struct {
    uint8_t     alloc_type:2;
    uint8_t     cache_type:2;
    uint8_t     write_policy:1;
    uint8_t     reserved:3;
} CACHE_NODE_ATTR;

typedef enum {
    CACHE_SCOPE_PRIVATE = 0x0,
    CACHE_SCOPE_CLUSTER,
    CACHE_SCOPE_SYSTEM
} CACHE_NODE_SCOPE;

/*
 * @brief   Cache Node Hierarchy Information
 *          bits[31:24] -   Indicates if the cache node scope is PE, CLUSTER or SYSTEM
 *          bits[23:0]  -   Provides more information for node scope
 *              = Don't care if node_scope == SYSTEM
 *              = (Aff3 <<16) | (Aff2 << 8) | (Aff1 << 0), if node_scope = CLUSTER
 *              = Aff0, if node_scope = PE
 */
typedef struct {
    uint32_t    scope_index:24;
    uint32_t    node_scope:8;
} CACHE_NODE_INFO;

typedef enum {
    INTR_EDGE_TRIGGER = 0x0,
    INTR_LEVEL_TRIGGER
} INTR_TRIGGER_TYPE;

typedef struct {
    uint32_t            error_intr_num;
    uint32_t            overflow_intr_num;
    INTR_TRIGGER_TYPE   error_intr_type;
    INTR_TRIGGER_TYPE   overflow_intr_type;
} INTR_INFO;

/*
 * @brief   Mpam Cache Node Entry
 */
typedef struct {
    uint16_t        line_size;
    uint32_t        size;
    uint64_t        neighbours[4];
    CACHE_NODE_INFO info;
    CACHE_NODE_ATTR attributes;
    addr_t          hwreg_base_addr;
    uint32_t        not_ready_max_us;
    INTR_INFO       intr_info;
} CACHE_NODE_ENTRY;

/*
 * @brief   Mpam Memory Node Entry
 */
typedef struct {
    uint32_t    proximity_domain;
    uint64_t    base_address;
    uint64_t    length;
    uint32_t    flags;
    addr_t      hwreg_base_addr;
    uint32_t    not_ready_max_us;
    INTR_INFO   intr_info;
} MEMORY_NODE_ENTRY;

typedef enum {
    MPAM_NODE_SMMU = 0x0,
    MPAM_NODE_CACHE,
    MPAM_NODE_MEMORY
} MPAM_NODE_TYPE;

/*
 * Mpam Cache Node Array Structure
 */
typedef struct {
    uint32_t            num_cache_nodes;
    uint32_t            num_memory_nodes;
    //uint32_t          num_smmu_nodes;
    CACHE_NODE_ENTRY    *cache_node;
    MEMORY_NODE_ENTRY   *memory_node;
    //SMMU_NODE_ENTRY   *smmu_node;
} MPAM_INFO_TABLE;

void pal_pe_create_info_table(PE_INFO_TABLE *pe_info_table);
void pal_mpam_create_info_table(MPAM_INFO_TABLE **mpam_info_table);

void pal_pe_call_smc(ARM_SMC_ARGS *args);
void pal_pe_execute_payload(ARM_SMC_ARGS *args);
uint32_t pal_pe_install_esr(uint32_t exception_type, void (*esr)(uint64_t, void *));

void pal_gic_create_info_table(GIC_INFO_TABLE *gic_info_table);
uint32_t pal_gic_install_isr(uint32_t int_id, void (*isr)(void));
uint32_t pal_gic_end_of_interrupt(uint32_t int_id);

uint64_t pal_pmu_reg_read(uint32_t reg_id);
void pal_pmu_reg_write(uint32_t reg_id, uint64_t write_data);
void pal_pmu_cycle_counter_start();
void pal_pmu_cycle_counter_stop();

/* Generic PAL function declarations */
void pal_print(char8_t *string, uint64_t data);
void pal_print_raw(addr_t addr, char8_t *string, uint64_t data);

void *pal_mem_alloc(uint32_t size);
void pal_mem_free(void *buffer);

void pal_mem_allocate_shared(uint32_t num_pe, uint32_t sizeofentry);
uint8_t pal_mem_allocate_shared_memcpybuf(uint64_t mem_base, uint64_t mem_size, uint64_t buf_size, uint32_t pe_cnt);
void pal_mem_allocate_shared_latencybuf(uint32_t node_cnt, uint32_t scenario_cnt);
void pal_mem_free_shared(void);
void pal_mem_free_shared_memcpybuf(uint32_t pe_cnt, uint64_t buf_size);
void pal_mem_free_shared_latencybuf(uint32_t node_cnt);
uint64_t pal_mem_get_shared_addr(void);
uint64_t pal_mem_get_shared_memcpybuf_addr(void);
uint64_t pal_mem_get_shared_latencybuf_addr(void);
void *pal_mem_allocate_buf(uint64_t size);
void *pal_mem_allocate_address(uint64_t mem_base, uint64_t mem_size, uint64_t buf_size);
void pal_mem_copy(void *source_addr,void *destination_addr, uint64_t length);
void pal_mem_free_buf(void *buffer, uint64_t size);

uint32_t pal_mmio_read(addr_t addr);
void pal_mmio_write(addr_t addr, uint32_t data);

void pal_pe_update_elr(void *context, uint64_t offset);
uint64_t pal_pe_get_esr(void *context);
uint64_t pal_pe_get_far(void *context);
void pal_pe_data_cache_ops_by_va(addr_t addr, uint32_t type);

#endif

