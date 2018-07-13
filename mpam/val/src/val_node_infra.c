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
#include "include/val_cache.h"
#include "include/val_memory.h"
#include "include/val_node_infra.h"
#include "include/val_mpam_hwreg_defs.h"


MPAM_INFO_TABLE *g_mpam_info_table;
extern PE_INFO_TABLE *g_pe_info_table;
uint32_t err_ctrl_reg;


uint64_t val_sysreg_read(sysreg_mpam_t regid)
{
    switch(regid)
    {
        case MPAMIDR_SYSREG:
#if !MPAM_SIMULATION_FVP
            return sysreg_read_mpamidr();
#else
            return MPAMIDR_EL1_FVP;
#endif
        case MPAM1_SYSREG:
#if !MPAM_SIMULATION_FVP
            return sysreg_read_mpam1();
#else
            return 0;
#endif
        case MPAM2_SYSREG:
#if !MPAM_SIMULATION_FVP
            return sysreg_read_mpam2();
#else
            return 0;
#endif
    }
    return 0;
}

void val_sysreg_write(sysreg_mpam_t regid, uint64_t data)
{

    switch(regid)
    {
        case MPAM2_SYSREG:
#if !MPAM_SIMULATION_FVP
            sysreg_write_mpam2(data);
#endif
            break;
        default:
            break;
    }
    return;
}


/**
 * @brief   This API will call PAL layer to fill in the MPAM information
 *          into the g_mpam_info_table pointer.
 *          1. Caller       -  Application layer.
 *          2. Prerequisite -  Memory allocated and passed as argument.
 * @param   None
 * @return  Error if Input param is NULL or num_mpam_cache_nodes is 0.
 */
uint32_t val_mpam_create_info_table()
{

    g_mpam_info_table = NULL;
    pal_mpam_create_info_table(&g_mpam_info_table);
    val_data_cache_ops_by_va((addr_t)&g_mpam_info_table, CLEAN_AND_INVALIDATE);

    val_print(ACS_PRINT_TEST, " MPAM_INFO: Number of MPAM Cache Nodes: %4d \n", val_node_get_total(MPAM_NODE_CACHE));
    val_print(ACS_PRINT_TEST, " MPAM_INFO: Number of MPAM Memory Nodes : %4d \n", val_node_get_total(MPAM_NODE_MEMORY));

    if (val_node_get_total(MPAM_NODE_CACHE) == 0) {
        val_print(ACS_PRINT_ERR, "\n *** CRITICAL ERROR: Num MPAM Cache Nodes is 0x0 ***\n", 0);
        return ACS_STATUS_ERR;
    }
    return ACS_STATUS_PASS;
}

void val_mpam_free_info_table()
{
    uint32_t pe_index;

    for(pe_index = 0; pe_index < g_pe_info_table->header.num_of_pe; ++pe_index) {
        pal_mem_free((void*)g_mpam_info_table[pe_index].cache_node);
        pal_mem_free((void*)g_mpam_info_table[pe_index].memory_node);
        //pal_mem_free((void*)g_mpam_info_table[pe_index].smmu_node);
    }
    pal_mem_free((void *) g_mpam_info_table);
}

/**
 * @brief   This API returns the number of cache nodes from the g_mpam_info_table.
 *          1. Caller       -  Application layer, test Suite.
 *          2. Prerequisite -  val_mpam_create_info_table.
 * @param   none
 * @return  The number of mpam cache nodes discovered
 */
uint32_t val_node_get_total(uint8_t node_type)
{

    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    switch (node_type) {
        case MPAM_NODE_CACHE :
            return g_mpam_info_table[pe_index].num_cache_nodes;
        case MPAM_NODE_MEMORY :
            return g_mpam_info_table[pe_index].num_memory_nodes;
        case MPAM_NODE_SMMU :
        default :
            return 0;
    }
}

/**
 * @brief   This API returns the hwreg base address for cache nodes from the g_mpam_info_table.
 *          1. Caller       -  Application layer, test Suite.
 *          2. Prerequisite -  val_mpam_create_info_table.
 * @param   none
 * @return  The base address of a mpam cache node hardware register bank
 */
addr_t  val_node_hwreg_base(uint8_t node_type, uint32_t node_index)
{

    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    switch (node_type) {
        case MPAM_NODE_CACHE :
            return g_mpam_info_table[pe_index].cache_node[node_index].hwreg_base_addr;
        case MPAM_NODE_MEMORY :
            return g_mpam_info_table[pe_index].memory_node[node_index].hwreg_base_addr;
        case MPAM_NODE_SMMU :
        default :
            return 0;
    }
}

uint16_t val_node_get_partid(uint8_t node_type, uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(node_type, node_index);
    return (val_mmio_read(base + REG_MPAMF_IDR) & IDR_PARTID_MAX_MASK);
}

uint8_t val_node_get_pmg(uint8_t node_type, uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(node_type, node_index);
    return ((val_mmio_read(base + REG_MPAMF_IDR) >> IDR_PMG_MAX_SHIFT) & IDR_PMG_MAX_MASK);
}

/**
 * @brief   Returns error interrupt number for MPAM nodes from g_mpam_info_table
 *
 * @param   node_type   - Indicates the MPAM node type
 * @param   node_index  - Index into the corresponding node array
 * @return  GSIV for Err interrupt, zero if this is not implemented
 */
uint32_t  val_node_get_error_intrnum(uint8_t node_type, uint32_t node_index)
{
    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    switch (node_type) {
        case MPAM_NODE_CACHE :
            return g_mpam_info_table[pe_index].cache_node[node_index].intr_info.error_intr_num;
        case MPAM_NODE_MEMORY :
            return g_mpam_info_table[pe_index].memory_node[node_index].intr_info.error_intr_num;
        case MPAM_NODE_SMMU :
        default :
            return 0;
    }
}

/**
 * @brief   Returns error interrupt type for MPAM nodes from g_mpam_info_table
 *
 * @param   node_type   - Indicates the MPAM node type
 * @param   node_index  - Index into the corresponding node array
 * @return  GSIV for Err interrupt, zero if this is not implemented
 */
uint32_t  val_node_get_error_intrtype(uint8_t node_type, uint32_t node_index)
{
    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0xFFFF;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    switch (node_type) {
        case MPAM_NODE_CACHE :
            return g_mpam_info_table[pe_index].cache_node[node_index].intr_info.error_intr_type;
        case MPAM_NODE_MEMORY :
            return g_mpam_info_table[pe_index].memory_node[node_index].intr_info.error_intr_type;
        case MPAM_NODE_SMMU :
        default :
            return 0;
    }
}

/**
 * @brief   Returns overflow interrupt number for MPAM nodes from g_mpam_info_table
 *
 * @param   node_type   - Indicates the MPAM node type
 * @param   node_index  - Index into the corresponding node array
 * @return  GSIV for OFlow interrupt, zero if this is not implemented
 */
uint32_t  val_node_get_oflow_intrnum(uint8_t node_type, uint32_t node_index)
{
    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    switch (node_type) {
        case MPAM_NODE_CACHE :
            return g_mpam_info_table[pe_index].cache_node[node_index].intr_info.overflow_intr_num;
        case MPAM_NODE_MEMORY :
            return g_mpam_info_table[pe_index].memory_node[node_index].intr_info.overflow_intr_num;
        case MPAM_NODE_SMMU :
        default :
            return 0;
    }
}

/**
 * @brief   Returns overflow interrupt type for MPAM nodes from g_mpam_info_table
 *
 * @param   node_type   - Indicates the MPAM node type
 * @param   node_index  - Index into the corresponding node array
 * @return  GSIV for OFlow interrupt, zero if this is not implemented
 */
uint32_t  val_node_get_oflow_intrtype(uint8_t node_type, uint32_t node_index)
{
    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0xFFFF;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    switch (node_type) {
        case MPAM_NODE_CACHE :
            return g_mpam_info_table[pe_index].cache_node[node_index].intr_info.overflow_intr_type;
        case MPAM_NODE_MEMORY :
            return g_mpam_info_table[pe_index].memory_node[node_index].intr_info.overflow_intr_type;
        case MPAM_NODE_SMMU :
        default :
            return 0;
    }
}

void val_node_trigger_intr(uint8_t node_type, uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(node_type, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    /* Trigger error interrupt by writing non-zero to MPAMF_ESR.ERRCODE */
    val_mmio_write(base + REG_MPAMF_ESR, (MPAM_ERRCODE_PARTID_SEL_RANGE << ESR_ERRCODE_SHIFT));

    val_memory_ops_issue_barrier(DSB);
}

void val_node_clear_intr(uint8_t node_type, uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(node_type, node_index);
    val_mmio_write(base + REG_MPAMF_ESR, 0x0);
    val_memory_ops_issue_barrier(DSB);
}

void val_node_restore_ecr(uint8_t node_type, uint32_t node_index)
{

    addr_t base = val_node_hwreg_base(node_type, node_index);

    /* Restore MPAMF_ECR register settings */
    val_mmio_write(base + REG_MPAMF_ECR, err_ctrl_reg);
    val_memory_ops_issue_barrier(DSB);
}

void val_node_generate_psr_error(uint8_t node_type, uint32_t node_index)
{

    addr_t base;
    uint16_t max_partid;

    base = val_node_hwreg_base(node_type, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Extract max PARTID supported by this MSC */
    max_partid = val_node_get_partid(node_type, node_index);

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    /* Write (max_partid+1) to PART_SEL register to generate PSR error */
    val_mmio_write(base + REG_MPAMCFG_PART_SEL, max_partid+1);

    val_memory_ops_issue_barrier(DSB);
}

uint8_t val_node_supports_mon(uint8_t node_type, uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(node_type, node_index);
    return ((val_mmio_read(base + REG_MPAMF_IDR) >> IDR_HAS_MSMON_SHIFT) & IDR_HAS_MSMON_MASK);
}

void val_node_generate_msmon_oflow_error(uint32_t node_index, uint16_t mon_count)
{

    addr_t base;
    void *src_buf = 0;
    void *dest_buf = 0;
    uint64_t buf_size;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    /* Write (mon_count-1) to MON_SEL register to configure the last monitor */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_count-1);

    /* Configure monitor filter reg for default partid and default pmg */
    val_mmio_write(base + REG_MSMON_CFG_MBWU_FLT, ((DEFAULT_PMG << MBWU_FLT_PMG_SHIFT)| DEFAULT_PARTID));

    /* Configure monitor ctrl reg for default partid and pmg to generate a oflow interrupt */
    val_mmio_write(base + REG_MSMON_CFG_MBWU_CTL, ((1 << MBWU_CTL_ENABLE_MATCH_PARTID_SHIFT) |
                                                   (1 << MBWU_CTL_ENABLE_MATCH_PMG_SHIFT) |
                                                   (COUNT_BW_RW << MBWU_CTL_SELECT_SUBTYPE_SHIFT) |
                                                   (1 << MBWU_CTL_ENABLE_OFLOW_FRZ_SHIFT) |
                                                   (1 << MBWU_CTL_ENABLE_OFLOW_INTR_SHIFT)
                                                   ));

    /* Write the maximum memory bandwidth usage value to MSMON_MBWU register */
    val_mmio_write(base + REG_MSMON_MBWU, ((0 << MBWU_CAPTURE_NRDY_SHIFT) | MBWU_VALUE_MAX));

    val_memory_ops_issue_barrier(DSB);

    /* Create two MB buffers sufficient to cretae overflow for this memory channel */
    buf_size = TWO_MB;
    src_buf = val_allocate_address(val_memory_get_base(node_index),
                                   val_memory_get_size(node_index),
                                   buf_size
                                   );
    dest_buf = val_allocate_address(val_memory_get_base(node_index),
                                    val_memory_get_size(node_index),
                                    buf_size
                                   );

    if ((src_buf == NULL) || (dest_buf == NULL)) {
        val_print(ACS_PRINT_ERR, "\n       Mem allocation for MBWPBM buffers failed", 0x0);
        return;
    }

    /* Start mem copy to cause the overflow interrupt */
    val_mem_copy(src_buf, dest_buf, buf_size);

    /* Free the buffers after return from overflow interrupt */
    val_free_buf(src_buf, buf_size);
    val_free_buf(dest_buf, buf_size);
}

void val_node_generate_msmon_config_error(uint8_t node_type, uint32_t node_index, uint16_t mon_count)
{

    addr_t base;
    uint16_t max_partid;

    base = val_node_hwreg_base(node_type, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Extract max PARTID supported by this MSC */
    max_partid = val_node_get_partid(node_type, node_index);

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    /* Write (mon_count-1) to MON_SEL register to configure the last monitor */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_count-1);

    switch (node_type) {
        case MPAM_NODE_CACHE:
            /* Write (max_partid+1) to CSU monitor filter reg to raise monitor config error */
            val_mmio_write(base + REG_MSMON_CFG_CSU_FLT, max_partid+1);
            break;
        case MPAM_NODE_MEMORY:
            /* Write (max_partid+1) to MBW monitor filter reg to raise monitor config error */
            val_mmio_write(base + REG_MSMON_CFG_MBWU_FLT, max_partid+1);
            break;
        default:
            mon_count = 0;
            break;
    }

    val_memory_ops_issue_barrier(DSB);
}

void val_node_generate_msr_error(uint8_t node_type, uint32_t node_index, uint16_t mon_count)
{

    addr_t base;

    base = val_node_hwreg_base(node_type, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    /* Write (mon_count+1) to MON_SEL register to generate MSR error */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_count+1);

    val_memory_ops_issue_barrier(DSB);
}

uint32_t val_node_generate_por_error(uint8_t node_type, uint32_t node_index)
{

    addr_t base;
    uint16_t msc_max_partid;
    uint16_t pe_max_partid;
    uint64_t mpamidr;
    uint64_t mpam2_el2 = 0;
    void *src_buf = 0;
    void *dest_buf = 0;

    base = val_node_hwreg_base(node_type, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Extract max PARTID supported by this MSC */
    msc_max_partid = val_node_get_partid(node_type, node_index);

    /* Extract max PARTID supported by MPAMIDR_EL1 */
    mpamidr = val_sysreg_read(MPAMIDR_SYSREG);

    pe_max_partid = (mpamidr >> MPAMIDR_PARTID_MAX_SHIFT) & MPAMIDR_PARTID_MAX_MASK;

    /* Check if MSC PARTID support exceeds PE PARTID generation range */
    if (msc_max_partid > pe_max_partid) {
        return ACS_STATUS_SKIP;
    }

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    /* Write MSC out-of-range PARTID (msc_max_partid+1) to MPAM2_EL2 */
    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);
    mpam2_el2 |= ((msc_max_partid+1) << MPAMn_ELx_PARTID_D_SHIFT);
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    /* Create buffers to perform memcopy with out-of-range PARTID */
    src_buf = val_allocate_buf(POR_BUF_SIZE);
    dest_buf = val_allocate_buf(POR_BUF_SIZE);

    /* Start mem copy transaction to generate POR error interrupt */
    val_mem_copy(src_buf, dest_buf, POR_BUF_SIZE);

    return ACS_STATUS_PASS;
}

uint32_t val_node_generate_pmgor_error(uint8_t node_type, uint32_t node_index)
{

    addr_t base;
    uint16_t msc_max_pmg;
    uint16_t pe_max_pmg;
    uint64_t mpamidr;
    uint64_t mpam2_el2 = 0;
    void *src_buf = 0;
    void *dest_buf = 0;

    base = val_node_hwreg_base(node_type, node_index);
    err_ctrl_reg = val_mmio_read(base + REG_MPAMF_ECR);

    /* Extract max PMG supported by this MSC */
    msc_max_pmg = val_node_get_pmg(node_type, node_index);

    /* Extract max PMG supported by MPAMIDR_EL1 */
    mpamidr = val_sysreg_read(MPAMIDR_SYSREG);
    pe_max_pmg = (mpamidr >> MPAMIDR_PMG_MAX_SHIFT) & MPAMIDR_PMG_MAX_MASK;

    /* Check if MSC PMG support exceeds PE PMG generation range */
    if (msc_max_pmg > pe_max_pmg) {
        val_print(ACS_PRINT_WARN, "\n node_type is %d \n", node_type);
        val_print(ACS_PRINT_WARN, "\n node_index is %d \n", node_index);
        val_print(ACS_PRINT_WARN, "\n Skipping test as MSC pmg > PE pmg \n", 0);
        return ACS_STATUS_SKIP;
    }

    /* Set interrupt enable bit in MPAMF_ECR */
    val_mmio_write(base + REG_MPAMF_ECR, (1 << ECR_ENABLE_INTEN_SHIFT));

    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);

    /* Clear the PARTID_D & PMG_D bits in mpam2_el2 before writing to them */
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PARTID_D_SHIFT+15, MPAMn_ELx_PARTID_D_SHIFT);
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PMG_D_SHIFT+7, MPAMn_ELx_PMG_D_SHIFT);

    /* Write DEFAULT_PARTID & (MSC_MAX_PMG + 1) to MPAM2_EL2 and generate PE traffic */
    mpam2_el2 |= (((uint64_t)(msc_max_pmg+1) << MPAMn_ELx_PMG_D_SHIFT) |
                  ((uint64_t)DEFAULT_PARTID << MPAMn_ELx_PARTID_D_SHIFT));

    /* Write MSC out-of-range PMG (msc_max_pmg+1) to MPAM2_EL2 */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    /* Create buffers to perform memcopy with out-of-range PMG */
    src_buf = val_allocate_buf(POR_BUF_SIZE);
    dest_buf = val_allocate_buf(POR_BUF_SIZE);

    /* Start mem copy transaction to generate PMGOR error interrupt */
    val_mem_copy(src_buf, dest_buf, PMGOR_BUF_SIZE);

    return ACS_STATUS_PASS;
}
