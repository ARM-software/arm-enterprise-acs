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


#define TEST_NUM   ACS_CACHE_TEST_NUM_BASE  +  1
#define TEST_DESC  "Check MPAM node presence          "

static void payload()
{

    uint32_t pe_index;
    uint32_t total_nodes;

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    total_nodes = val_node_get_total(MPAM_NODE_CACHE) +
                  val_node_get_total(MPAM_NODE_MEMORY);

    /* Skip the entire compliance suite if no MPAM nodes present in the system */
    if (total_nodes == 0) {
        val_print(ACS_PRINT_ERR, "\n\n     Zero MPAM suported nodes in the system \n", 0);
        val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 01));
        return;
    }

    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    return;
}

uint32_t testc001_entry()
{

    uint32_t status = ACS_STATUS_FAIL;
    uint32_t num_pe = 1;

    status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);

    val_run_test_payload(TEST_NUM, num_pe, payload, 0);

    /* get the result from all PE and check for failure */
    status = val_check_for_error(TEST_NUM, num_pe);

    val_report_status(0, ACS_TEST_END(TEST_NUM));

    return status;
}
