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

#ifndef __PAL_UEFI_H__
#define __PAL_UEFI_H__

#include "pal_interface.h"
//#include <Library/UefiLib.h>

/**
 * @brief   Structure to hold memory info from memory affinity table
*/
typedef struct {
  UINT64 BaseAddr;
  UINT64 Length;
  UINT32 Flags;
} MEMORY_AFFINITY_FIELDS;

/**
 * @brief   Structure to hold type and length
*/
typedef struct {
  UINT8 Type;
  UINT8 Length;
} SRAT_TYPE_LENGTH;

/**
 *@brief PPTT NODE HEADER (Common fields in all PPTT tables)
 */
typedef struct{
  UINT8 Type;
  UINT8 Length;
} PPTT_NODE_HEADER;

UINT64 pal_get_xsdt_ptr();
UINT64 pal_get_pptt_ptr();
UINT64 pal_get_mpam_ptr();
UINT64 pal_get_madt_ptr();
UINT64 pal_get_srat_ptr();
EFI_STATUS   pal_get_srat_info();

extern VOID* g_acs_log_file_handle;
extern UINT32 g_print_level;

/* Only Errors. Use this to de-clutter the terminal and focus only on specifics */
#define ACS_PRINT_ERR   5
/* Only warnings & errors. Use this to de-clutter the terminal and focus only on specifics */
#define ACS_PRINT_WARN  4
/* Test description and result descriptions. THIS is DEFAULT */
#define ACS_PRINT_TEST  3
/* For Debug statements. Contains register dumps etc */
#define ACS_PRINT_DEBUG 2
/* Print all statements. Do not use unless really needed */
#define ACS_PRINT_INFO  1

#define acs_print(verbose, string, ...) if(verbose >= g_print_level) \
                                            Print(string, ##__VA_ARGS__)

#endif
