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

#include "val/include/val_infra.h"
#include "val/include/val_cache.h"
#include "val/include/val_csu_monitor.h"
#include "val/include/val_node_infra.h"
#include "val/include/val_mpam_hwreg_defs.h"


#define TEST_NUM   ACS_CSUMON_TEST_NUM_BASE +  3
#define TEST_DESC  "Check new monitor config doesn't affect ongoing"

#define NODE_CNT_MAX 1024 //(2^10), permissible max = (2^16)
#define MON_CNT_MAX 256 //(2^8,) permissible max = (2^16)
#define PARTITION_PERCENTAGE 75
#define CACHE_PERCENTAGE 50
#define MAX_DEVIATION_CNT 10

static uint64_t mpam2_el2_temp;

static void payload()
{

    uint8_t pmg1;
    uint8_t pmg2;
    uint8_t minmax_pmg;
    uint8_t csu_index;
    uint16_t moncnt;
    uint16_t max_moncnt;
    uint16_t mon_index;
    uint32_t node_index;
    uint32_t node_index2;
    uint32_t pe_index;
    uint32_t total_nodes;
    uint32_t cache_node_size = 0;
    void *src_buf = 0;
    void *dest_buf = 0;
    uint64_t buf_size;
    uint32_t **storage_value1;
    uint32_t **storage_value2;
    uint64_t mpam2_el2 = 0;

    minmax_pmg = DEFAULT_PMG_MAX;
    max_moncnt = DEFAULT_MONCNT;
    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    total_nodes = val_node_get_total(MPAM_NODE_CACHE);


    /* Compute minmax pmg & max_moncnt among all MPAM nodes */
    for(node_index = 0; node_index < total_nodes; node_index++) {

        minmax_pmg = GET_MIN_VALUE (minmax_pmg,
                                    val_node_get_pmg(MPAM_NODE_CACHE, node_index)
                                    );

        if (val_cache_supports_csumon(node_index)) {
            max_moncnt = GET_MAX_VALUE (max_moncnt,
                                        val_csumon_monitor_count(node_index)
                                        );
        }
    }

    val_print(ACS_PRINT_DEBUG, "     minmax_pmg          = %d\n", minmax_pmg);
    val_print(ACS_PRINT_DEBUG, "     max_moncnt          = %d\n", max_moncnt);

    /* Skip this test if none of the cache nodes support monitoring */
    if (max_moncnt == DEFAULT_MONCNT) {
        val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 0));
        return;
    }

    /* Create two PMG groups for PE traffic */
    pmg1 = minmax_pmg;
    pmg2 = minmax_pmg-1;

    /* Dynamically create the two cache storage usage buffers,
     * uint32_t storage_value1[MON_CNT_MAX][NODE_CNT_MAX] and
     * uint32_t storage_value2[MON_CNT_MAX][NODE_CNT_MAX],
     * potentially non contiguous
     */
    storage_value1 = (uint32_t **) val_allocate_buf(MON_CNT_MAX * sizeof (uint32_t *));
    storage_value2 = (uint32_t **) val_allocate_buf(MON_CNT_MAX * sizeof (uint32_t *));

    for (mon_index = 0; mon_index < MON_CNT_MAX; mon_index ++) {
        storage_value1[mon_index] = val_allocate_buf(NODE_CNT_MAX * sizeof (uint32_t));
        storage_value2[mon_index] = val_allocate_buf(NODE_CNT_MAX * sizeof (uint32_t));
    }

    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);
    mpam2_el2_temp = mpam2_el2;

    /*
     * Loop over all cache nodes and over their monitors and check
     * if the configuration of a new monitor affect the ongoing monitor
     */
    for (node_index = 0; node_index < total_nodes; node_index++) {

        /* Configure the partitioning parameters for all cache nodes */
        for (node_index2 = 0; node_index2 < total_nodes; node_index2++) {

            if (node_index2 != node_index) {

                if (val_cache_supports_cpor(node_index2)) {
                    /* Disable CPOR partitioning for DEFAULT_PARTID */
                    val_cache_configure_cpor(node_index2, DEFAULT_PARTID, 100);
                }

                if (val_cache_supports_ccap(node_index2)) {
                    /* Disable CCAP partitioning for DEFAULT_PARTID */
                    val_cache_configure_ccap(node_index2, DEFAULT_PARTID, 0, 100);
                }
            } else {

                if (val_cache_supports_cpor(node_index)) {
                    /* Configure CPOR partitioning for DEFAULT_PARTID */
                    val_cache_configure_cpor(node_index,
                                             DEFAULT_PARTID,
                                             PARTITION_PERCENTAGE
                                             );
                }

                if (val_cache_supports_ccap(node_index)) {
                    /* Configure CCAP partitioning for DEFAULT_PARTID */
                    val_cache_configure_ccap(node_index,
                                             DEFAULT_PARTID,
                                             HARDLIMIT_EN,
                                             PARTITION_PERCENTAGE
                                             );
                }

                /* Read the size of this cache node */
                cache_node_size = val_cache_get_size(node_index);

                /* Read the csu monitor count supported by this cache node */
                if (val_cache_supports_csumon(node_index)) {
                    moncnt = val_csumon_monitor_count(node_index);
                } else {
                    moncnt = 0;
                }
            }
        }

        for (mon_index = 0; mon_index < moncnt; mon_index++) {

            /* Configure the first monitor for this cache node */
            val_csumon_config_monitor(node_index, DEFAULT_PARTID, pmg1, mon_index);

            /* Clear the PARTID_D & PMG_D bits in mpam2_el2 before writing to them */
            mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PARTID_D_SHIFT+15, MPAMn_ELx_PARTID_D_SHIFT);
            mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PMG_D_SHIFT+7, MPAMn_ELx_PMG_D_SHIFT);

            /* Write DEFAULT_PARTID & PMG1 to MPAM2_EL2 and generate PE traffic */
            mpam2_el2 |= (((uint64_t)pmg1 << MPAMn_ELx_PMG_D_SHIFT) |
                           ((uint64_t)DEFAULT_PARTID << MPAMn_ELx_PARTID_D_SHIFT));
            val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

            /* Set the buffer size for memcopy */
            buf_size = cache_node_size * CACHE_PERCENTAGE / 100 / 2;
            val_print(ACS_PRINT_DEBUG, "\n     mon_index           = 0x%x\n", mon_index);
            val_print(ACS_PRINT_DEBUG, "     node_index          = 0x%lx\n", node_index);
            val_print(ACS_PRINT_DEBUG, "     cache_node_size     = 0x%x\n", cache_node_size);
            val_print(ACS_PRINT_DEBUG, "     buf_size            = 0x%x\n", buf_size);

            /* Init the csu_value for this monitor to zero for PMG1*/
            storage_value1[mon_index][node_index] = 0;

            for (csu_index = 0; csu_index < MAX_DEVIATION_CNT; csu_index++) {

                /* Create buffers to perform memcopy (stream copy) */
                src_buf = val_allocate_buf(buf_size);
                dest_buf = val_allocate_buf(buf_size);

                if ((src_buf == NULL) || (dest_buf == NULL)) {
                    /* Restore monitor control register settings */
                    val_csumon_restore_ctlreg(node_index, mon_index);
                    val_print(ACS_PRINT_ERR, "\n       Mem allocation for buffer creation failed", 0x0);
                    val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 01));
                }

                /* Clear the cache storage usage register */
                val_csumon_reset_monitor(node_index, mon_index);

                /* Start mem copy to measure cache storage usage */
                val_mem_copy(src_buf, dest_buf, buf_size);

                /* Keep track of cache storage usage valuer from the cache monitor reg */
#if !MPAM_SIMULATION_FVP
                storage_value1[mon_index][node_index] = GET_MIN_VALUE(
                                                        storage_value1[mon_index][node_index],
                                                        val_csumon_storage_value(node_index, mon_index
                                                        ));
#else
                storage_value1[mon_index][node_index] = 0;
#endif

                /* Free the copy buffers to the heap manager */
                val_free_buf(src_buf, buf_size);
                val_free_buf(dest_buf, buf_size);
            }

            val_print(ACS_PRINT_DEBUG, "     storage_value1      = 0x%lx\n",
                                             storage_value1[mon_index][node_index]);

            /* Clear the PARTID_D & PMG_D bits in mpam2_el2 before writing to them */
            mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PARTID_D_SHIFT+15, MPAMn_ELx_PARTID_D_SHIFT);
            mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PMG_D_SHIFT+7, MPAMn_ELx_PMG_D_SHIFT);

            /* Write DEFAULT_PARTID & PMG2 to MPAM2_EL2 and generate PE traffic */
            mpam2_el2 |= (((uint64_t)pmg2 << MPAMn_ELx_PMG_D_SHIFT) |
                          ((uint64_t)DEFAULT_PARTID << MPAMn_ELx_PARTID_D_SHIFT));
            val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

            /* Init the csu_value for this monitor to zero for PMG2*/
            storage_value2[mon_index][node_index] = 0;

            /* Create buffers to perform memcopy (stream copy) */
            src_buf = val_allocate_buf(buf_size);
            dest_buf = val_allocate_buf(buf_size);

            /* Clear the cache storage usage register */
            val_csumon_reset_monitor(node_index, mon_index);

            /* Perform half mem copy before configuring the second monitor */
            val_mem_copy(src_buf, dest_buf, buf_size/2);

            /* Restore monitor control register settings */
            val_csumon_restore_ctlreg(node_index, mon_index);

            /* Configure the second monitor for this cache node */
            val_csumon_config_monitor(node_index, DEFAULT_PARTID, pmg2, mon_index);

            /* Perform second half mem copy after configuring second monitor */
            val_mem_copy(src_buf, dest_buf, buf_size/2);

            /* Read cache storage usage register from the cache monitor */
#if !MPAM_SIMULATION_FVP
            storage_value2[mon_index][node_index] = GET_MIN_VALUE(
                                                    storage_value2[mon_index][node_index],
                                                    val_csumon_storage_value(node_index, mon_index
                                                    ));
#else
            storage_value2[mon_index][node_index] = 0;
#endif

            /* Free the copy buffers to the heap manager */
            val_free_buf(src_buf, buf_size);
            val_free_buf(dest_buf, buf_size);

            /* Restore monitor control register settings */
            val_csumon_restore_ctlreg(node_index, mon_index);

            val_print(ACS_PRINT_DEBUG, "     storage_value2      = 0x%lx\n",
                                             storage_value2[mon_index][node_index]);
        }

    }

    /* Restore MPAM2_EL2 settings */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2_temp);

    /* Compare cache storage usage values for all enabled monitors of all cache node */
    for (node_index = 0; node_index <  total_nodes; node_index++) {

        /* Read the csu monitor count supported by this cache node */
        if (val_cache_supports_csumon(node_index)) {
            moncnt = val_csumon_monitor_count(node_index);
        } else {
             moncnt = 0;
        }

        for (mon_index = 0; mon_index < moncnt; mon_index++) {

            if (storage_value1[mon_index][node_index] > storage_value2[mon_index][node_index]) {

                val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 01));

                /* Free the csu storage 2D buffers to the heap manager */
                for (mon_index = 0; mon_index < MON_CNT_MAX; mon_index ++) {
                    val_free_buf(storage_value1[mon_index], (NODE_CNT_MAX * sizeof (uint32_t)));
                    val_free_buf(storage_value2[mon_index], (NODE_CNT_MAX * sizeof (uint32_t)));
                }
                return;
            }
        }
    }

    /* Free the csu storage 2D buffers to the heap manager */
    for (mon_index = 0; mon_index < MON_CNT_MAX; mon_index ++) {
        val_free_buf(storage_value1[mon_index], (NODE_CNT_MAX * sizeof (uint32_t)));
        val_free_buf(storage_value2[mon_index], (NODE_CNT_MAX * sizeof (uint32_t)));
    }

    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    return;
}

uint32_t testm003_entry()
{

    uint32_t status = ACS_STATUS_FAIL;
    uint32_t num_pe = 1;

    status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);

    /* This check is when user is forcing us to skip this test */
    if (status != ACS_STATUS_SKIP)
        val_run_test_payload(TEST_NUM, num_pe, payload, 0);

    /* get the result from all PE and check for failure */
    status = val_check_for_error(TEST_NUM, num_pe);

    val_report_status(0, ACS_TEST_END(TEST_NUM));

    return status;
}
