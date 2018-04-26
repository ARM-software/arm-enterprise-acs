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

#include "val_interface.h"
#include "pal_interface.h"

wd_info_table_t *g_wd_info_table;

/**
 *  @brief   This API is a single point of entry to retrieve
 *           information stored in the WD Info table
 *           1. Caller       -  Test Suite
 *           2. Prerequisite -  val_wd_create_info_table
 *  @param   type   the type of information being requested
 *  @return  64-bit data
 */
uint64_t val_wd_get_info(uint32_t index, WD_INFO_TYPE info_type)
{

  if (g_wd_info_table == NULL)
      return 0;

    switch (info_type) {
        case WD_INFO_COUNT:
            return g_wd_info_table->header.num_wd;
        case WD_INFO_CTRL_BASE:
            return g_wd_info_table->wd_info[index].wd_ctrl_base;
        case WD_INFO_REFRESH_BASE:
            return g_wd_info_table->wd_info[index].wd_refresh_base;
        case WD_INFO_GSIV:
            return g_wd_info_table->wd_info[index].wd_gsiv;
        case WD_INFO_ISSECURE:
            return ((g_wd_info_table->wd_info[index].wd_flags >> 2) & 1);
        default:
            return 0;
    }
}

/**
 *  @brief   This API creates the watchdog info table.
 *
 *  @param wd_info_table pre-allocated memory pointer for wd_info
 *
 *  @return  status
 */
int val_wd_create_info_table(uint64_t *wd_info_table)
{
    int32_t ret = 0;

    if (wd_info_table == NULL) {
        val_print(ACS_LOG_ERR, "\n        Input for Create Info table cannot be NULL");
        return 1;
    }

    g_wd_info_table = (wd_info_table_t *)wd_info_table;
    ret = pal_wd_create_info_table(g_wd_info_table);

    val_print(ACS_LOG_INFO, "\n        WATCHDOG_INFO: Number of Watchdogs   : %lld",
                                                        val_wd_get_info(0, WD_INFO_COUNT));
    val_pe_data_cache_clean_invalidate((uint64_t)&g_wd_info_table);

    return ret;
}

/**
 *  @brief   This API Enables watchdog by writing to Control Base register
 *  @param   index  identifies the watchdog instance to enable
 *  @return  None
 */
void val_wd_enable(uint32_t index)
{
  val_mmio_write(g_wd_info_table->wd_info[index].wd_ctrl_base, 1);
}

/**
 *  @brief   This API disables watchdog by writing to Control Base register
 *  @param   index  identifies the watchdog instance to enable
 *  @return  None
 */
void val_wd_disable(uint32_t index)
{
  val_mmio_write(g_wd_info_table->wd_info[index].wd_ctrl_base, 0);
}

/**
 *  @brief   This API arms the watchdog by writing to Control Base register
 *  @param   index   identifies the watchdog instance to program
 *  @param   timeout ticks to generation of ws0 interrupt
 *  @return  None
 */
void val_wd_set_ws0(uint64_t *vaddr, uint32_t index, uint32_t timeout)
{
    pal_wd_set_ws0(vaddr, g_wd_info_table->wd_info[index].wd_ctrl_base, index, timeout);
}

/**
 *  @brief   This function frees the allocated memory for Watchdog info table
 *  @param   None
 *  @return  none
 */
void val_watchdog_free_info_table(void)
{
    pal_free_mem((uint64_t *)g_wd_info_table);
}
