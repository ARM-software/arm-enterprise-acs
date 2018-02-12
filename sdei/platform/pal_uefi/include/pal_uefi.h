/*
 * SDEI ACS Platform module.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2018 Arm Limited
 * SPDX-License-Identifier : Apache-2.0
 *
 *
 */

#ifndef __PAL_UEFI_H__
#define __PAL_UEFI_H__

#include "pal_interface.h"

extern sdei_log_control g_log_control;

typedef struct {
  UINT64   Arg0;
  UINT64   Arg1;
  UINT64   Arg2;
  UINT64   Arg3;
  UINT64   Arg4;
  UINT64   Arg5;
  UINT64   Arg6;
  UINT64   Arg7;
} ARM_SMC_ARGS;


#define ARM_SMC_ID_PSCI_CPU_ON_AARCH64         0xc4000003
#define ARM_SMC_ID_PSCI_CPU_OFF                0x84000002
#define ARM_SMC_ID_PSCI_CPU_SUSPEND_AARCH64    0xc4000001
#define ARM_SMC_ID_PSCI_FEATURES               0x8400000A
#define BASE_FVP_UART_BASE                     0x1c090000

VOID
DataCacheCleanInvalidateVA(UINT64 Address);
VOID
DataCacheInvalidateVA(UINT64 Address);

VOID
pal_print(UINT32 verbosity, CHAR8 *str, ...);

#endif /* __PAL_UEFI_H__ */
