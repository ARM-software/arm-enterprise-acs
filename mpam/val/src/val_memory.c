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
#include "include/val_memory.h"
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
uint32_t val_memory_execute_tests(uint32_t num_pe)
{

    uint32_t status, i;

    for (i=0 ; i< MAX_TEST_SKIP_NUM ; i++) {
        if (g_skip_test_num[i] == ACS_CACHE_TEST_NUM_BASE) {
            val_print(ACS_PRINT_TEST, "\n USER Override - Skipping all memory partition tests \n", 0);
            return ACS_STATUS_SKIP;
        }
    }

    status = testd001_entry();
    status |= testd002_entry();
    status |= testd003_entry();

    if (status != ACS_STATUS_PASS)
        val_print(ACS_PRINT_TEST, "\n      *** One or more memory partition tests have failed... *** \n", 0);
    else
        val_print(ACS_PRINT_TEST, "\n      All memory partition tests have passed!! \n", 0);

    return status;
}

/**
 * @brief   This API checks if the MSC supports Memory Bandwidth partitioning
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_IDR.HAS_MBW_PART bit value
 */
uint8_t val_memory_supports_part(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    return ((val_mmio_read(base + REG_MPAMF_IDR) >> IDR_HAS_MBW_PART_SHIFT) & IDR_HAS_MBW_PART_MASK);
}

/**
 * @brief   This API checks if the MSC supports Minimum Memory Bandwidth partitioning
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBW_IDR.HAS_MIN bit value
 */
uint8_t val_memory_supports_mbwmin(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_part(node_index))
        return ((val_mmio_read(base + REG_MPAMF_MBW_IDR) >> MBW_IDR_HAS_MIN_SHIFT) & MBW_IDR_HAS_MIN_MASK);
    else
        return 0;

}

/**
 * @brief   This API checks if the MSC supports Maximum Memory Bandwidth partitioning
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBW_IDR.HAS_MAX bit value
 */
uint8_t val_memory_supports_mbwmax(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_part(node_index))
        return ((val_mmio_read(base + REG_MPAMF_MBW_IDR) >> MBW_IDR_HAS_MAX_SHIFT) & MBW_IDR_HAS_MAX_MASK);
    else
        return 0;

}

/**
 * @brief   This API checks if the MSC supports Memory Bandwidth Portion partitioning
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBW_IDR.HAS_PBM bit value
 */
uint8_t val_memory_supports_mbwpbm(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_part(node_index))
        return ((val_mmio_read(base + REG_MPAMF_MBW_IDR) >> MBW_IDR_HAS_PBM_SHIFT) & MBW_IDR_HAS_PBM_MASK);
    else
        return 0;

}

/**
 * @brief   This API returns the width of the Minimum Memory Bandwidth partitioning bit map
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBW_IDR.BWA_WD bit value
 */
uint16_t val_memory_mbwmin_width(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_part(node_index))
        return ((val_mmio_read(base + REG_MPAMF_MBW_IDR) >> MBW_IDR_BWA_WD_SHIFT) & MBW_IDR_BWA_WD_MASK);
    else
        return 0;
}

/**
 * @brief   This API returns the width of the Maximum Memory Bandwidth partitioning bit map
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBW_IDR.BWA_WD bit value
 */
uint16_t val_memory_mbwmax_width(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_part(node_index))
        return ((val_mmio_read(base + REG_MPAMF_MBW_IDR) >> MBW_IDR_BWA_WD_SHIFT) & MBW_IDR_BWA_WD_MASK);
    else
        return 0;
}

/**
 * @brief   This API returns the width of the Memory Bandwidth Portion partitioning bit map
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBW_IDR.BWPBM_WD bit value
 */
uint16_t val_memory_mbwpbm_width(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_part(node_index))
        return ((val_mmio_read(base + REG_MPAMF_MBW_IDR) >> MBW_IDR_BWPBM_WD_SHIFT) & MBW_IDR_BWPBM_WD_MASK);
    else
        return 0;
}

/**
 * @brief   This API checks if the MSC supports Memory Bandwidth Usage monitor
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MSMON_IDR.MSMON_MBWU bit value
 */
uint8_t val_memory_supports_mbwumon(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_node_supports_mon(MPAM_NODE_MEMORY, node_index))
        return ((val_mmio_read(base + REG_MPAMF_MSMON_IDR) >> MSMON_IDR_MSMON_MBWU_SHIFT) & MSMON_IDR_MSMON_MBWU_MASK);
    else
        return 0;
}

/**
 * @brief   This API returns the number of memory bandwidth monitors supported by an MSC
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_MBWUMON_IDR.NUM_MON bit value
 */
uint16_t val_memory_mon_count(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    if (val_memory_supports_mbwumon(node_index))
        return (val_mmio_read(base + REG_MPAMF_MBWUMON_IDR) & MBWUMON_IDR_NUM_MON_MASK);
    else
        return 0;
}

void val_memory_configure_mbwmin(uint32_t node_index, uint16_t partid, uint32_t mbwmin_percentage)
{

    addr_t base;
    uint8_t num_fractional_bits;
    uint16_t fixed_point_fraction;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    num_fractional_bits = val_memory_mbwmin_width(node_index);
    fixed_point_fraction = ((1 << num_fractional_bits) * mbwmin_percentage / 100) - 1;

    /* Select the PARTID to configure minimum bandwidth limit parameters */
    val_mmio_write(base + REG_MPAMCFG_PART_SEL, partid);

    /*
     * Configure the MBW_MIN register for minimum bandwidth limit.
     * Use num_fractional_bits fixed-point representation
     */
    val_mmio_write(base + REG_MPAMCFG_MBW_MIN,
                  ((fixed_point_fraction << (16 - num_fractional_bits)) & 0xFFFF));

    val_memory_ops_issue_barrier(DSB);
    return;
}

void val_memory_configure_mbwmax(uint32_t node_index, uint16_t partid, uint8_t hardlim, uint32_t mbwmax_percentage)
{

    addr_t base;
    uint8_t num_fractional_bits;
    uint16_t fixed_point_fraction;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    num_fractional_bits = val_memory_mbwmax_width(node_index);
    fixed_point_fraction = ((1 << num_fractional_bits) * mbwmax_percentage / 100) - 1;

    /* Select the PARTID to configure maximum bandwidth partition parameters */
    val_mmio_write(base + REG_MPAMCFG_PART_SEL, partid);

    /*
     * Configure the MBW_MAX register for maximum bandwidth limit.
     * Use num_fractional_bits fixed-point representation
     */
    val_mmio_write(base + REG_MPAMCFG_MBW_MAX, (hardlim << MBW_MAX_HARDLIM_SHIFT) |
              ((fixed_point_fraction << (16 - num_fractional_bits)) & 0xFFFF));

    val_memory_ops_issue_barrier(DSB);
    return;
}

void val_memory_configure_mbwpbm(uint32_t node_index, uint16_t partid, uint32_t mbwpbm_percentage)
{

    addr_t base;
    uint16_t index;
    uint32_t unset_bitmask;
    uint32_t num_unset_bits;
    uint16_t num_mbwpbm_bits;

    base = val_node_hwreg_base(MPAM_NODE_MEMORY, node_index);
    num_mbwpbm_bits = val_memory_mbwpbm_width(node_index);

    /* Select the PARTID to configure portion partition parameters */
    val_mmio_write(base + REG_MPAMCFG_PART_SEL, partid);

    /*
     * Configure MBWPBM register to have a 1 in mbwpbm_percentage
     * bits in the overall MBWBM_WD bit positions
     */
    num_mbwpbm_bits = num_mbwpbm_bits * mbwpbm_percentage / 100;
    for (index = 0; index < num_mbwpbm_bits-31; index += 32)
        val_mmio_write(base + REG_MPAMCFG_MBW_PBM + index, MBWPOR_BITMAP_DEF_VAL);

    num_unset_bits = num_mbwpbm_bits - index;
    unset_bitmask = (1 << num_unset_bits) - 1;
    if(unset_bitmask)
        val_mmio_write(base + REG_MPAMCFG_MBW_PBM + index, unset_bitmask);

    val_memory_ops_issue_barrier(DSB);
    return;
}

/**
 * @brief   This API returns the base address of the input memory node
 *
 * @param   node_index  - index into global mpam info memory table
 * @return  base address of the input memory node
 */
uint64_t val_memory_get_base(uint32_t node_index)
{

    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    return g_mpam_info_table[pe_index].memory_node[node_index].base_address;
}

/**
 * @brief   This API returns the size the input memory node
 *
 * @param   node_index  - index into global mpam info memory table
 * @return  size of the input memory node
 */
uint64_t val_memory_get_size(uint32_t node_index)
{

    uint32_t pe_index;

    if (g_mpam_info_table == NULL) {
         return 0;
    }

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    return g_mpam_info_table[pe_index].memory_node[node_index].length;
}
