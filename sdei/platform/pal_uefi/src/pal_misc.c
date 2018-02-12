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
#include  <Library/ShellCEntryLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellLib.h>
#include  <Library/PrintLib.h>

#include "pal_uefi.h"

/**
  @brief  Sends a formatted string to the output console

  @param  string  An ASCII string
  @param  data    data for the formatted output

  @return None
**/
VOID
pal_print(UINT32 verbosity, CHAR8 *str, ...) {
  if (verbosity <= g_log_control.print_level) {

    CHAR8 Buffer[2048];
    UINTN BufferSize = 1;
    if (str) {
        VA_LIST marker;
        VA_START(marker, str);
        BufferSize = AsciiVSPrint(Buffer, 2048, str, marker);
        VA_END(marker);
    }

    AsciiPrint(Buffer);

    if (g_log_control.log_file_handle) {

        EFI_STATUS Status = 0;
        Status = ShellWriteFile(g_log_control.log_file_handle, &BufferSize, (VOID*)Buffer);
        if (EFI_ERROR(Status))
            AsciiPrint("Error in writing to log file\n");
    }
  }
}

/**
  @brief  Provides a single point of abstraction to read from all
          Memory Mapped IO address

  @param  addr 64-bit address

  @return 32-bit data read from the input address
**/
UINT32
pal_mmio_read(UINT64 addr)
{
  UINT32 data;

  if (addr & 0x3) {
      addr = addr & ~(0x3);  //make sure addr is aligned to 4 bytes
  }
  data = (*(volatile UINT32 *)addr);
  return data;
}

/**
  @brief  Provides a single point of abstraction to write to all
          Memory Mapped IO address

  @param  addr  64-bit address
  @param  data  32-bit data to write to address

  @return None
**/
VOID
pal_mmio_write(UINT64 addr, UINT32 data)
{
  *(volatile UINT32 *)addr = data;
}

UINT64 *pal_pa_to_va(UINT64 addr)
{
    return 0;
}

VOID pal_va_write(UINT64 *addr, UINT32 offset, UINT32 data)
{

}

VOID pal_va_free(UINT64 *addr)
{

}

VOID
pal_print_raw(CHAR8 *string, UINT64 data)
{
    UINT8 j, buffer[16];
    INT8  i=0;
    /* TODO : Fix THis */
    UINT64 addr = BASE_FVP_UART_BASE;
    for (;*string!='\0';++string) {
        if (*string == '%') {
            ++string;
            if (*string == 'd') {
                while (data != 0) {
                    j = data%10;
                    data = data/10;
                    buffer[i]= j + 48 ;
                    i = i+1;
                }
            } else if (*string == 'x' || *string == 'X') {
                while (data != 0) {
                    j = data & 0xf;
                    data = data >> 4;
                    buffer[i]= j + ((j > 9) ? 55 : 48) ;
                    i = i+1;
                }
            }
            if (i>0) {
                while (i>=0)
                    *(volatile UINT8 *)addr = buffer[--i];
            } else
                *(volatile UINT8 *)addr = 48;

        } else
            *(volatile UINT8 *)addr = *string;
    }
}
