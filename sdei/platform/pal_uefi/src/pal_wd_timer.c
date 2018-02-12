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

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#include <Protocol/AcpiTable.h>
#include "Include/IndustryStandard/Acpi61.h"

#include "include/pal_uefi.h"

#define INVALID_INDEX 0xFFFFFFFF

typedef enum {
    TIMER_INFO_IS_PLATFORM_TIMER_SECURE,
    TIMER_INFO_SYS_INTID,
    TIMER_INFO_SYS_CNT_BASE_N
} sys_timer_info_t;

timer_info_table_t *g_sys_timertable;

static EFI_ACPI_6_1_GENERIC_TIMER_DESCRIPTION_TABLE *gGtdtHdr;

UINT64
pal_get_gtdt_ptr();

/**
  @brief  This API fills in the TIMER_INFO_TABLE with information about local and system
          timers in the system. This is achieved by parsing the ACPI - GTDT table.

  @param  TimerTable  - Address where the Timer information needs to be filled.

  @return  None
**/
VOID
pal_timer_create_info_table(timer_info_table_t *TimerTable)
{
  EFI_ACPI_6_1_GTDT_GT_BLOCK_STRUCTURE       *Entry = NULL;
  EFI_ACPI_6_1_GTDT_GT_BLOCK_TIMER_STRUCTURE *GtBlockTimer = NULL;
  timer_info_gtblock_t        *GtEntry = NULL;
  UINT32                      Length= 0;
  UINT32                      i;
  UINT32                      num_of_entries;

  if (TimerTable == NULL) {
    pal_print(ACS_LOG_ERR, "\n        Input Timer Table Pointer is NULL.");
    return;
  }

  GtEntry = TimerTable->gt_info;
  TimerTable->header.num_platform_timer = 0;

  gGtdtHdr = (EFI_ACPI_6_1_GENERIC_TIMER_DESCRIPTION_TABLE *) pal_get_gtdt_ptr();

  if (gGtdtHdr == NULL) {
    pal_print(ACS_LOG_ERR, "\n        GTDT not found");
    return;
  }
  pal_print(ACS_LOG_INFO, "\n        GTDT is at %x and length is %x",
                                                        gGtdtHdr, gGtdtHdr->Header.Length);

  /* Fill in our internal table */
  TimerTable->header.s_el1_timer_flag  = gGtdtHdr->SecurePL1TimerFlags;
  TimerTable->header.ns_el1_timer_flag = gGtdtHdr->NonSecurePL1TimerFlags;
  TimerTable->header.el2_timer_flag    = gGtdtHdr->NonSecurePL2TimerFlags;
  TimerTable->header.s_el1_timer_gsiv  = gGtdtHdr->SecurePL1TimerGSIV;
  TimerTable->header.ns_el1_timer_gsiv = gGtdtHdr->NonSecurePL1TimerGSIV;
  TimerTable->header.el2_timer_gsiv    = gGtdtHdr->NonSecurePL2TimerGSIV;
  TimerTable->header.virtual_timer_flag = gGtdtHdr->VirtualTimerFlags;
  TimerTable->header.virtual_timer_gsiv = gGtdtHdr->VirtualTimerGSIV;

  Length         = gGtdtHdr->PlatformTimerOffset;
  Entry          = (EFI_ACPI_6_1_GTDT_GT_BLOCK_STRUCTURE *) ((UINT8 *)gGtdtHdr + Length);
  Length         = sizeof (EFI_ACPI_6_1_GENERIC_TIMER_DESCRIPTION_TABLE);
  num_of_entries = gGtdtHdr->PlatformTimerCount;

  while (num_of_entries) {

    if (Entry->Type == EFI_ACPI_6_1_GTDT_GT_BLOCK) {
      pal_print(ACS_LOG_INFO, "\n        Found block entry");
      GtEntry->type = TIMER_TYPE_SYS_TIMER;
      GtEntry->block_cntl_base = Entry->CntCtlBase;
      GtEntry->timer_count     = Entry->GTBlockTimerCount;
      pal_print(ACS_LOG_DEBUG, "\n        CNTCTLBase = %x", GtEntry->block_cntl_base);
      GtBlockTimer = (EFI_ACPI_6_1_GTDT_GT_BLOCK_TIMER_STRUCTURE *)(((UINT8 *)Entry) +
                                                                    Entry->GTBlockTimerOffset);
      for (i = 0; i < GtEntry->timer_count; i++) {
        pal_print(ACS_LOG_INFO, "\n        Found timer entry");
        GtEntry->GtCntBase[i]    = GtBlockTimer->CntBaseX;
        GtEntry->GtCntEl0Base[i] = GtBlockTimer->CntEL0BaseX;
        GtEntry->gsiv[i]         = GtBlockTimer->GTxPhysicalTimerGSIV;
        GtEntry->virt_gsiv[i]    = GtBlockTimer->GTxVirtualTimerGSIV;
        GtEntry->flags[i]        = GtBlockTimer->GTxPhysicalTimerFlags |
                                   (GtBlockTimer->GTxVirtualTimerFlags << 8) |
                                   (GtBlockTimer->GTxCommonFlags << 16);
        DataCacheCleanInvalidateVA((UINT64)GtEntry);
        pal_print(ACS_LOG_DEBUG, "\n        CNTBaseN = %x for sys counter = %d",
                                                                GtEntry->GtCntBase[i], i);
        GtBlockTimer++;
        TimerTable->header.num_platform_timer++;
      }
      GtEntry++;
    }
    Entry = (EFI_ACPI_6_1_GTDT_GT_BLOCK_STRUCTURE *) ((UINT8 *)Entry + (Entry->Length));
    num_of_entries--;
  }
  DataCacheCleanInvalidateVA((UINT64)TimerTable);
  g_sys_timertable = TimerTable;
}

/**
  @brief  This API fills in the wd_info_table_t with information about Watchdogs
          in the system. This is achieved by parsing the ACPI - GTDT table.

  @param  WdTable  - Address where the Timer information needs to be filled.

  @return  None
**/

UINT32
pal_wd_create_info_table(wd_info_table_t *WdTable)
{

  EFI_ACPI_6_1_GTDT_SBSA_GENERIC_WATCHDOG_STRUCTURE    *Entry = NULL;
  wd_info_entry_t             *WdEntry = NULL;
  UINT32                      Length= 0;
  UINT32                      num_of_entries;

  if (WdTable == NULL) {
    pal_print(ACS_LOG_ERR, "\n        Input Watchdog Table Pointer is NULL.");
    return 1;
  }

  WdEntry = WdTable->wd_info;
  WdTable->header.num_wd = 0;
  gGtdtHdr = (EFI_ACPI_6_1_GENERIC_TIMER_DESCRIPTION_TABLE *) pal_get_gtdt_ptr();

  if (gGtdtHdr == NULL) {
    pal_print(ACS_LOG_ERR, "\n        GTDT not found");
    return 1;
  }

  Length         = gGtdtHdr->PlatformTimerOffset;
  Entry          = (EFI_ACPI_6_1_GTDT_SBSA_GENERIC_WATCHDOG_STRUCTURE *) ((UINT8 *)gGtdtHdr +
                            Length);
  Length         = sizeof (EFI_ACPI_6_1_GENERIC_TIMER_DESCRIPTION_TABLE);
  num_of_entries = gGtdtHdr->PlatformTimerCount;

  while (num_of_entries)
  {

    if (Entry->Type == EFI_ACPI_6_1_GTDT_GT_BLOCK) {
      //Skip. this info is added in the timer info function
    }

    if (Entry->Type == EFI_ACPI_6_1_GTDT_SBSA_GENERIC_WATCHDOG) {
      WdEntry->wd_refresh_base = Entry->RefreshFramePhysicalAddress;
      WdEntry->wd_ctrl_base    = Entry->WatchdogControlFramePhysicalAddress;
      WdEntry->wd_gsiv         = Entry->WatchdogTimerGSIV;
      WdEntry->wd_flags        = Entry->WatchdogTimerFlags;
      DataCacheCleanInvalidateVA((UINT64)WdEntry);
      WdTable->header.num_wd++;
      pal_print(ACS_LOG_INFO, "\n        Watchdog base = 0x%x INTID = 0x%x",
                                WdEntry->wd_ctrl_base, WdEntry->wd_gsiv);
      WdEntry++;
    }
    Entry = (EFI_ACPI_6_1_GTDT_SBSA_GENERIC_WATCHDOG_STRUCTURE *)((UINT8 *)Entry + (Entry->Length));
    num_of_entries--;

  }
  DataCacheCleanInvalidateVA((UINT64)WdTable);

  return 0;
}

VOID pal_wd_set_ws0(UINT64 *vaddr, UINT64 base, UINT32 index, UINT32 timeout)
{
    if (timeout == 0) {
      pal_mmio_write((base + WD_REG_BASE), WD_DISABLE); //val_wd_disable(index);
      return;
    }

    pal_mmio_write((base + WD_REG_CTRL), timeout);
    pal_mmio_write((base + WD_REG_BASE), WD_ENABLE); //val_wd_enable(index);
}

VOID pal_platform_timer_get_entry_index(UINT32 index, UINT32 *block, UINT32 *block_index)
{
    if (index > g_sys_timertable->header.num_platform_timer){
        *block = INVALID_INDEX;
        return;
    }
    *block = 0;
    *block_index = index;
    while (index > g_sys_timertable->gt_info[*block].timer_count) {
        index = index - g_sys_timertable->gt_info[*block].timer_count;
        *block_index   = index;
        *block   = *block + 1;
    }
}

UINT64 pal_timer_get_info(UINT32 type, UINT32 index)
{
    UINT32 block_num, block_index;

    if (!g_sys_timertable)
        return INVALID_INDEX;

    switch (type) {
        case TIMER_INFO_IS_PLATFORM_TIMER_SECURE:
            pal_platform_timer_get_entry_index (index, &block_num, &block_index);
            if (block_num != INVALID_INDEX)
                return ((g_sys_timertable->gt_info[block_num].flags[block_index] >> 16) & 1);
            break;
        case TIMER_INFO_SYS_INTID:
            pal_platform_timer_get_entry_index (index, &block_num, &block_index);
            if (block_num != INVALID_INDEX)
                return g_sys_timertable->gt_info[block_num].gsiv[block_index];
            break;
        case TIMER_INFO_SYS_CNT_BASE_N:
            pal_platform_timer_get_entry_index (index, &block_num, &block_index);
            if (block_num != INVALID_INDEX)
                return g_sys_timertable->gt_info[block_num].GtCntBase[block_index];
            break;
        default:
            break;
    }

    return INVALID_INDEX;
}

acs_status_t pal_configure_second_interrupt(UINT32 *index, UINT64 *int_id)
{
    UINT64 timer_num = g_sys_timertable->header.num_platform_timer;
    UINT32 ns_timer= 0;

    if (!timer_num) {
        pal_print(ACS_LOG_ERR, "\n        No System timers are defined");
        return ACS_ERROR;
    }

    while (timer_num && !ns_timer) {
        /* array index starts from 0, so subtract 1 from count */
        timer_num--;
        /* Skip secure timer */
        if (pal_timer_get_info(TIMER_INFO_IS_PLATFORM_TIMER_SECURE, timer_num))
            continue;

        ns_timer++;

        *int_id = pal_timer_get_info(TIMER_INFO_SYS_INTID, timer_num);
        *index = timer_num;
    }

    if (!ns_timer) {
        pal_print(ACS_LOG_ERR, "\n        No non-secure systimer implemented");
        return ACS_ERROR;
    }

    return ACS_SUCCESS;
}

VOID pal_generate_second_interrupt(UINT32 index, UINT32 timeout)
{
    UINT64 cnt_base_n;
    cnt_base_n = pal_timer_get_info(TIMER_INFO_SYS_CNT_BASE_N, index);
    pal_mmio_write(cnt_base_n + 0x28, timeout);
    pal_mmio_write(cnt_base_n + 0x2C, 1);
}

VOID pal_disable_second_interrupt(UINT32 index)
{
    UINT64 cnt_base_n;
    cnt_base_n = pal_timer_get_info(TIMER_INFO_SYS_CNT_BASE_N, index);
    pal_mmio_write(cnt_base_n + 0x2C, 0);
}
