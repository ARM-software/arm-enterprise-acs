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
#include "include/val_interrupts.h"
#include "include/val_node_infra.h"
#include "include/val_mpam_hwreg_defs.h"


extern MPAM_INFO_TABLE *g_mpam_info_table;

/**
 * @brief   This API will execute all PE tests designated for a given compliance level
 *          1. Caller       -  Application layer.
 * @param   num_pe - the number of PE to run these tests on.
 * @return  Consolidated status of all the tests run.
 */
uint32_t val_interrupts_execute_tests(uint32_t num_pe)
{

    uint32_t status, i;

    for (i=0 ; i<MAX_TEST_SKIP_NUM ; i++) {
        if (g_skip_test_num[i] == ACS_INTR_TEST_NUM_BASE) {
            val_print(ACS_PRINT_TEST, "\n USER Override - Skipping all error interrupt tests \n", 0);
            return ACS_STATUS_SKIP;
        }
    }

    status = testi001_entry();
    status |= testi002_entry();
    status |= testi003_entry();
    status |= testi004_entry();
    status |= testi005_entry();
    status |= testi006_entry();
    status |= testi007_entry();
    status |= testi008_entry();

    if (status != ACS_STATUS_PASS)
        val_print(ACS_PRINT_TEST, "\n      *** One or more error interrupt tests have failed... *** \n", 0);
    else
        val_print(ACS_PRINT_TEST, "\n      All error interrupt tests have passed!! \n", 0);

    return status;
}
