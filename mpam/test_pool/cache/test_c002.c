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
#include "val/include/val_node_infra.h"
#include "val/include/val_mpam_hwreg_defs.h"


#define TEST_NUM   ACS_CACHE_TEST_NUM_BASE  +  2
#define TEST_DESC  "Check CPOR Partitioning           "

#define CPOR_SCENARIO_MAX 10
static uint64_t mpam2_el2_temp;

typedef struct {
    char8_t description[64];
    uint8_t partition_percent;  //as function of max cpor cache size
    uint8_t cache_percent;      //as function of max cpor cache size
    uint8_t config_enable;
} cpor_config_t;

cpor_config_t cpor_config_data[] = {
    {"CPOR latency check for 75% partition size", 75, 75, TRUE},
    {"CPOR latency check for 50% partition size", 50, 75, FALSE},
    {"CPOR latency check for 25% partition size", 25, 75, TRUE}
};

static void payload()
{

    uint8_t enabled_scenarios = 0;
    uint16_t minmax_partid;
    uint16_t index;
    uint32_t node_index;
    uint32_t pe_index;
    uint32_t total_nodes;
    uint32_t cpor_cache_maxsize = 0;
    uint32_t cpor_nodes = 0;
    void *src_buf = 0;
    void *dest_buf = 0;
    uint64_t buf_size;
    uint64_t start_time;
    uint64_t end_time;
    uint64_t latency[CPOR_SCENARIO_MAX];
    uint64_t mpam2_el2 = 0;

    minmax_partid = DEFAULT_PARTID_MAX;
    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    total_nodes = val_node_get_total(MPAM_NODE_CACHE);

    /*
     * Compute the number of CPOR supported MPAM caches nodes,
     * the max size of the cache node that supports CPOR, and
     * the min partition id supported among all MPAM nodes
     */
    for(node_index = 0; node_index < total_nodes; node_index++) {

        if (val_cache_supports_cpor(node_index)) {
            cpor_cache_maxsize = GET_MAX_VALUE(cpor_cache_maxsize,
                                      val_cache_get_size(node_index));
            cpor_nodes++;
        }

        minmax_partid = GET_MIN_VALUE(minmax_partid,
                             val_node_get_partid(MPAM_NODE_CACHE, node_index));
    }

    val_print(ACS_PRINT_DEBUG, "\n\n     cpor_nodes          = %d\n", cpor_nodes);
    val_print(ACS_PRINT_DEBUG, "     minmax_partid       = %d\n", minmax_partid);

    /* Skip this test if no CPOR supported MPAM cache nodes present in the system */
    if (cpor_nodes == 0) {
        val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 0));
        return;
    }

    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);
    mpam2_el2_temp = mpam2_el2;

    /* Clear the PARTID_D & PMG_D bits in mpam2_el2 before writing to them */
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PARTID_D_SHIFT+15, MPAMn_ELx_PARTID_D_SHIFT);
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PMG_D_SHIFT+7, MPAMn_ELx_PMG_D_SHIFT);

    /* Write MINMAX_PARTID & DEFAULT PMG to mpam2_el2 to generate PE traffic */
    mpam2_el2 |= (((uint64_t)DEFAULT_PMG << MPAMn_ELx_PMG_D_SHIFT) |
                  ((uint64_t)minmax_partid << MPAMn_ELx_PARTID_D_SHIFT));

    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    for (index = 0; index < sizeof(cpor_config_data)/sizeof(cpor_config_t); index ++) {

        if (cpor_config_data[index].config_enable) {
            /*
             * Loop over all cache nodes and configure CPOR supported
             * cache nodes as per configuration data, disable CCAP
             * partitioning nodes
             */
            for (node_index = 0; node_index <total_nodes; node_index++) {

                if (val_cache_supports_cpor(node_index)) {
                    /* Configure CPOR partitioning for min(max(PARTID)) */
                    val_cache_configure_cpor(node_index, minmax_partid,
                              cpor_config_data[index].partition_percent);
                }

                if (val_cache_supports_ccap(node_index)) {
                    /* Disable CCAP partitioning for min(max(PARTID)) */
                    val_cache_configure_ccap(node_index, minmax_partid, 0, 100);
                }
            }

            /* Create buffers to perform memcopy (stream copy) */
            buf_size = cpor_cache_maxsize * cpor_config_data[index].cache_percent / 100 / 2;
            src_buf = val_allocate_buf(buf_size);
            dest_buf = val_allocate_buf(buf_size);

            val_print(ACS_PRINT_DEBUG, "\n     index               = %d\n", index);
            val_print(ACS_PRINT_DEBUG, "     cpor_cache_maxsize  = %d\n", cpor_cache_maxsize);
            val_print(ACS_PRINT_DEBUG, "     buf_size            = %d\n", buf_size);

            if ((src_buf == NULL) || (dest_buf == NULL)) {
                val_print(ACS_PRINT_ERR, "\n       Mem allocation for COPR buffers failed", 0x0);
                val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 01));

                /* Restore MPAM2_EL2 settings */
                val_sysreg_write(MPAM2_SYSREG, mpam2_el2_temp);
                return;
            }

            /* Start mem copy, do not measure copy latency */
            val_mem_copy(src_buf, dest_buf, buf_size);

            /* Repeat mem copy, measure copy latency this time */
            val_measurement_start();
            start_time = val_measurement_read();
            val_mem_copy(src_buf, dest_buf, buf_size);
            end_time = val_measurement_read();
            latency[enabled_scenarios++] = end_time - start_time;
            val_measurement_stop();

            val_print(ACS_PRINT_DEBUG, "     start_time          = 0x%lx\n", start_time);
            val_print(ACS_PRINT_DEBUG, "     end_time            = 0x%lx\n", end_time);
            val_print(ACS_PRINT_DEBUG, "     latency             = 0x%lx\n", latency[enabled_scenarios-1]);

            /* Free the buffers to the heap manager */
            val_free_buf(src_buf, buf_size);
            val_free_buf(dest_buf, buf_size);
        }
    }

    /* Restore MPAM2_EL2 settings */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2_temp);

    /* Compare the stream copy latencies for all the scenarios */
    for (index = 1; index < enabled_scenarios; index++) {

        if (latency[index] < latency[index-1]) {
            val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 01));
            return;
        }
    }

    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    return;
}

uint32_t testc002_entry()
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
