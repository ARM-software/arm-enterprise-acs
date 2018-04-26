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

/**
 *  @brief  This API calls PAL layer to read from a Memory address
 *          and return 32-bit data.
 *          1. Caller       - Test Suite
 *          2. Prerequisite - None.
 *
 *  @param addr   64-bit address
 *
 *  @return       32-bits of data
 */
uint32_t
val_mmio_read(uint64_t addr)
{
  return pal_mmio_read(addr);

}

/**
 *  @brief  This function will call PAL layer to write 32-bit data to
 *          a Memory address.
 *        1. Caller       - Test Suite
 *        2. Prerequisite - None.
 *
 *  @param addr   64-bit address
 *  @param data   32-bit data
 *
 *  @return       None
 */
void
val_mmio_write(uint64_t addr, uint32_t data)
{

  pal_mmio_write(addr, data);
}

/**
 * @brief  This function call PAL layer and converts physical address to
 *         virtual address.
 * @param addr    64-bit address
 *
 * @return    Virtual address
 */
uint64_t *val_pa_to_va(uint64_t addr)
{
    uint64_t *va;
    va = pal_pa_to_va(addr);

    return va;
}

/**
 *    @brief  This function call PAL layer and write 32-bit data to
 *            memory address.
 *    @param addr   64-bit address
 *    @param offset  offset
 *    @param data    32-bit data
 *
 *    @return     None
 */
void val_va_write(uint64_t *addr, uint32_t offset, uint32_t data)
{
    pal_va_write(addr, offset, data);
}

void val_va_free(uint64_t *addr)
{
    pal_va_free(addr);
}

void val_intf_lock(void)
{
    pal_intf_lock();
}

void val_intf_unlock(void)
{
    pal_intf_unlock();
}
