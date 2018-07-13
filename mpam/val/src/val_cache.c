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
#include "include/val_node_infra.h"
#include "include/val_mpam_hwreg_defs.h"

extern MPAM_INFO_TABLE *g_mpam_info_table;

/**
 * @brief   This API will execute all PE tests designated for a given compliance level
 *          1. Caller       -  Application layer.
 *          2. Prerequisite -  val_pe_create_info_table, val_allocate_shared_mem
 * @param   num_pe - the number of PE to run these tests on.
 * @return  Consolidated status of all the tests run.
 */
uint32_t val_cache_execute_tests(uint32_t num_pe)
{

    uint32_t status, i;

    for (i=0 ; i<MAX_TEST_SKIP_NUM ; i++) {
        if (g_skip_test_num[i] == ACS_CACHE_TEST_NUM_BASE) {
            val_print(ACS_PRINT_TEST, "\n USER Override - Skipping all cache partition tests \n", 0);
            return ACS_STATUS_SKIP;
        }
    }

    status = testc001_entry();
    if (status != ACS_STATUS_PASS) {
        return ACS_STATUS_EXIT;
    }

    status |= testc002_entry();
    status |= testc003_entry();
    status |= testc004_entry();
    status |= testc005_entry();
    status |= testc006_entry();

    if (status != ACS_STATUS_PASS)
        val_print(ACS_PRINT_TEST, "\n      *** One or more cache partition tests have failed... *** \n", 0);
    else
        val_print(ACS_PRINT_TEST, "\n      All cache partition tests have passed!! \n", 0);

    return status;
}

/**
 * @brief   This API checks if the MSC supports Cache Portion Partitioning
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_IDR.HAS_CPOR bit value
 */
uint8_t val_cache_supports_cpor(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    return ((val_mmio_read(base + REG_MPAMF_IDR) >> IDR_HAS_CPOR_PART_SHIFT) & IDR_HAS_CPOR_PART_MASK);
}

/**
 * @brief   This API checks if the MSC supports Cache Capacity Partitioning
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_IDR.HAS_CCAP bit value
 */
uint8_t val_cache_supports_ccap(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    return ((val_mmio_read(base + REG_MPAMF_IDR) >> IDR_HAS_CCAP_PART_SHIFT) & IDR_HAS_CCAP_PART_MASK);
}

/**
 * @brief   This API checks if the MSC supports Cache Storage Usage monitor
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MSMON_IDR.MSMON_CSU bit value
 */
uint8_t val_cache_supports_csumon(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    if (val_node_supports_mon(MPAM_NODE_CACHE, node_index))
        return ((val_mmio_read(base + REG_MPAMF_MSMON_IDR) >> MSMON_IDR_MSMON_CSU_SHIFT) & MSMON_IDR_MSMON_CSU_MASK);
    else
        return 0;
}

/**
 * @brief   This API returns the width of the Cache Portion Partitioning Bit Map
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_CPOR_IDR.CPBM_WD bit value
 */
uint16_t val_cache_cpbm_width(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    if (val_cache_supports_cpor(node_index))
        return (val_mmio_read(base + REG_MPAMF_CPOR_IDR) & CPOR_IDR_CPBM_WD_MASK);
    else
        return 0;
}

/**
 * @brief   This API returns the width of the Cache Maximum Capacity Partitioning Bit Map
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_CCAP_IDR.CMAX_WD bit value
 */
uint8_t val_cache_cmax_width(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    if (val_cache_supports_ccap(node_index))
        return (val_mmio_read(base + REG_MPAMF_CCAP_IDR) & CCAP_IDR_CMAX_WD_MASK);
    else
        return 0;
}

/**
 * @brief   This API returns the number of cache monitors supported by an MSC
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_CSUMON_IDR.NUM_MON bit value
 */
uint16_t val_cache_mon_count(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    if (val_cache_supports_csumon(node_index))
        return (val_mmio_read(base + REG_MPAMF_CSUMON_IDR) & CSUMON_IDR_NUM_MON_MASK);
    else
        return 0;
}

void val_cache_configure_cpor(uint32_t node_index, uint16_t partid, uint32_t cpbm_percentage)
{

    addr_t base;
    uint16_t index;
    uint32_t unset_bitmask;
    uint32_t num_unset_bits;
    uint16_t num_cpbm_bits;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    num_cpbm_bits = val_cache_cpbm_width(node_index);

    /* Select the PARTID to configure portion partition parameters */
    val_mmio_write(base + REG_MPAMCFG_PART_SEL, partid);

    /*
     * Configure CPBM register to have a 1 in cpbm_percentage
     * bits in the overall CPBM_WD bit positions
     */
    num_cpbm_bits = num_cpbm_bits * cpbm_percentage / 100;
    for (index = 0; index < num_cpbm_bits-31; index += 32)
        val_mmio_write(base + REG_MPAMCFG_CPBM + index, CPOR_BITMAP_DEF_VAL);

    num_unset_bits = num_cpbm_bits - index;
    unset_bitmask = (1 << num_unset_bits) - 1;
    if(unset_bitmask)
        val_mmio_write(base + REG_MPAMCFG_CPBM + index, unset_bitmask);

    val_memory_ops_issue_barrier(DSB);
    return;
}

void val_cache_configure_ccap(uint32_t node_index, uint16_t partid, uint8_t hardlim, uint32_t ccap_percentage)
{

    addr_t base;
    uint8_t num_fractional_bits;
    uint16_t fixed_point_fraction;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    num_fractional_bits = val_cache_cmax_width(node_index);
    fixed_point_fraction = ((1 << num_fractional_bits) * ccap_percentage / 100) - 1;

    /* Select the PARTID to configure capacity partition parameters */
    val_mmio_write(base + REG_MPAMCFG_PART_SEL, partid);

    /*
     * Configure the CMAX register for the max capacity.
     * Use num_fractional_bits fixed-point representation
     */
    val_mmio_write(base + REG_MPAMCFG_CMAX, (hardlim << MBW_MAX_HARDLIM_SHIFT) |
              ((fixed_point_fraction << (16 - num_fractional_bits)) & 0xFFFF));

    val_memory_ops_issue_barrier(DSB);
    return;
}

uint32_t val_cache_get_size(uint32_t node_index)
{

    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    return g_mpam_info_table[pe_index].cache_node[node_index].size;

}
