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
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ShellLib.h>
#include <Protocol/AcpiTable.h>
#include "Foundation/Efi/Guid/Acpi/Acpi.h"
#include "Include/IndustryStandard/Acpi61.h"

#include "include/pal_uefi.h"
#include "include/pal_interface.h"
/**
 * @brief   Use UEFI System Table to look up Acpi20TableGuid and returns the Xsdt Address
 * @param   None
 * @return  Returns 64-bit XSDT address
 */

UINT64
pal_get_xsdt_ptr (
    VOID
  )
{

  EFI_ACPI_6_1_ROOT_SYSTEM_DESCRIPTION_POINTER* Rsdp;
  UINT32                                        Index;

  for (Index = 0, Rsdp = NULL; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (&(gST->ConfigurationTable[Index].VendorGuid), &gEfiAcpiTableGuid) ||
       CompareGuid (&(gST->ConfigurationTable[Index].VendorGuid), &gEfiAcpi20TableGuid)) {
         /* A match was found */
         Rsdp = (EFI_ACPI_6_1_ROOT_SYSTEM_DESCRIPTION_POINTER *) gST->ConfigurationTable[Index].VendorTable;
         break;
      }
  }

  if (Rsdp == NULL) {
      return 0;
  } else {
      return((UINT64) (EFI_ACPI_6_1_ROOT_SYSTEM_DESCRIPTION_POINTER *) Rsdp->XsdtAddress);
  }
}

/**
 * @brief   Iterate through the tables pointed by XSDT and return MADT address
 * @param   None
 * @return  64-bit MADT address
 */
UINT64
pal_get_madt_ptr (
    VOID
  )
{

  EFI_ACPI_DESCRIPTION_HEADER   *Xsdt;
  UINT64                        *Entry64;
  UINT32                        Entry64Num;
  UINT32                        Idx;

  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *) pal_get_xsdt_ptr();
  if (Xsdt == NULL) {
    acs_print(ACS_PRINT_ERR, L"XSDT not found \n");
    return 0;
  }

  Entry64  = (UINT64 *)(Xsdt + 1);
  Entry64Num = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) >> 3;
  for (Idx = 0; Idx < Entry64Num; Idx++) {
    if (*(UINT32 *)(UINTN)(Entry64[Idx]) == EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE) {
      return (UINT64)(Entry64[Idx]);
    }
  }

  return 0;
}
