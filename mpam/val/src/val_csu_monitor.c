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
#include "include/val_csu_monitor.h"
#include "include/val_node_infra.h"
#include "include/val_mpam_hwreg_defs.h"


extern MPAM_INFO_TABLE *g_mpam_info_table;
static uint32_t csu_ctl_temp;

/**
 * @brief   This API will execute all PE tests designated for a given compliance level
 *          1. Caller       -  Application layer.
 * @param   num_pe - the number of PE to run these tests on.
 * @return  Consolidated status of all the tests run.
 */
uint32_t val_csumon_execute_tests(uint32_t num_pe)
{

    uint32_t status, i;

    for (i=0 ; i< MAX_TEST_SKIP_NUM ; i++) {

        if (g_skip_test_num[i] == ACS_CSUMON_TEST_NUM_BASE) {
            val_print(ACS_PRINT_TEST, "\n USER Override - Skipping all csu monitor tests \n", 0);
            return ACS_STATUS_SKIP;
        }
    }

    status = testm001_entry();
    status |= testm002_entry();
    status |= testm003_entry();

    if (status != ACS_STATUS_PASS)
        val_print(ACS_PRINT_TEST, "\n      *** One or more cache monitor tests have failed... *** \n", 0);
    else
        val_print(ACS_PRINT_TEST, "\n      All cache monitor tests have passed!! \n", 0);

    return status;
}

/**
 * @brief   This API returns number of csu monitors supported by an MSC
 *
 * @param   node_index  - MPAM feature page index for this MSC
 * @return  MPAMF_CSUMON_IDR.NUM_MON bit value
 */
uint16_t val_csumon_monitor_count(uint32_t node_index)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);
    if (val_cache_supports_csumon(node_index))
        return (val_mmio_read(base + REG_MPAMF_CSUMON_IDR) & CSUMON_IDR_NUM_MON_MASK);
    else
        return 0;
}

/**
 * @brief   This API configures csu monitor control register and enables monitoring
 *
 * @param   node_index  - MPAM feature page index for this MS
 * @param   partid      - PARTID to be used in CSU storage matching criteria
 * @param   pmg         - PMG to be used in CSU storage matching criteria
 * @return  None
 */
void val_csumon_config_monitor(uint32_t node_index, uint16_t partid, uint8_t pmg, uint16_t mon_sel)
{

    addr_t base;
    uint32_t csu_ctl_reg;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);

    /* Make monitor selection to program its configuration registers */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_sel);

    /* Clear storage usage value & set NRDY bit in cache storage usage register */
    val_mmio_write(base + REG_MSMON_CSU, (1 << CSU_NRDY_SHIFT));

    /* Configure the CSU monitor filter register for input PARTID & PMG */
    val_mmio_write(base + REG_MSMON_CFG_CSU_FLT, ((pmg << CSU_FLT_PMG_SHIFT) | partid));

    /* Disable the monitor indicated by mon_sel via CSU monitor control register */
    csu_ctl_temp = val_mmio_read(base + REG_MSMON_CFG_CSU_CTL);
    csu_ctl_reg = (csu_ctl_temp & (~(1 << CSU_CTL_ENABLE_SHIFT)));
    val_mmio_write(base + REG_MSMON_CFG_CSU_CTL, csu_ctl_reg);

    /* Configure the CSU monitor control register to match input PARTID & PMG,
     * overflow freeze, no overflow interrupt and to enable monitoring on mon_sel
     */
    csu_ctl_reg &= (~(1 << CSU_CTL_ENABLE_OFLOW_INTR_SHIFT));
    csu_ctl_reg |= ((1 << CSU_CTL_ENABLE_MATCH_PARTID_SHIFT) |
                    (1 << CSU_CTL_ENABLE_MATCH_PMG_SHIFT) |
                    (1 << CSU_CTL_ENABLE_SHIFT));
    val_mmio_write(base + REG_MSMON_CFG_CSU_CTL, csu_ctl_reg);

    val_memory_ops_issue_barrier(DSB);
    return;
}

/**
 * @brief   This API returns cache storage usage value in bytes
 *
 * @param   node_index  - MPAM feature page index for this MS
 * @param   mon_sel     - monitor index to be used to read csu
 * @return  cache storage usage value in bytes if (NRDY == 0),
            zero if (NRDY == 1)
 */
uint32_t val_csumon_storage_value(uint32_t node_index, uint16_t mon_sel)
{

    addr_t base;
    uint32_t csu_value;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);

    /* Make monitor selection to read its registers */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_sel);

    val_memory_ops_issue_barrier(DSB);

    csu_value = val_mmio_read(base + REG_MSMON_CSU);

    if ((csu_value >> CSU_NRDY_SHIFT) & CSU_NRDY_MASK) {
        return 0;
    } else {
        return csu_value & CSU_VALUE_MASK;
    }
}

/**
 * @brief   This API resets cache storage usage value
 *
 * @param   node_index  - MPAM feature page index for this MS
 * @param   mon_sel     - monitor index to clear csu_value
 * @return  none
 */
void val_csumon_reset_monitor(uint32_t node_index, uint16_t mon_sel)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);

    /* Make monitor selection to configure its registers */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_sel);

    val_memory_ops_issue_barrier(DSB);

    /* Clear storage usage value & set NRDY bit in cache storage usage reg */
    val_mmio_write(base + REG_MSMON_CSU, (1 << CSU_NRDY_SHIFT));
}

/**
 * @brief   This API writes backup value to the monitor control register
 *
 * @param   node_index  - MPAM feature page index for this MS
 * @param   mon_sel     - monitor id whose value needs to be restored
 * @return  None
 */
void val_csumon_restore_ctlreg(uint32_t node_index, uint16_t mon_sel)
{

    addr_t base;

    base = val_node_hwreg_base(MPAM_NODE_CACHE, node_index);

    /* Select the monitor to access its configuration registers */
    val_mmio_write(base + REG_MSMON_CFG_MON_SEL, mon_sel);

    /* Store the default value to CSU monitor control register */
    val_mmio_write(base + REG_MSMON_CFG_CSU_CTL, csu_ctl_temp);

    val_memory_ops_issue_barrier(DSB);
    return;
}
