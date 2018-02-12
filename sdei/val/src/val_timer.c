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
#include "val_timer.h"

timer_info_table_t *g_timer_info_table;

/**
 * @brief   This API is the single entry point to return all Timer related information
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  val_timer_create_info_table
 * @param   info_type  - Type of the information to be returned
 *
 * @return  64-bit data pertaining to the requested input type
 */

uint64_t val_timer_get_info(timer_info_t info_type, uint64_t instance)
{

  uint32_t block_num, block_index;
  if (g_timer_info_table == NULL)
      return 0;

  switch (info_type) {
      case TIMER_INFO_CNTFREQ:
          return ArmArchTimerReadReg(CntFrq);
      case TIMER_INFO_PHY_EL1_INTID:
          return g_timer_info_table->header.ns_el1_timer_gsiv;
      case TIMER_INFO_VIR_EL1_INTID:
          return g_timer_info_table->header.virtual_timer_gsiv;
      case TIMER_INFO_PHY_EL2_INTID:
          return g_timer_info_table->header.el2_timer_gsiv;
      case TIMER_INFO_VIR_EL2_INTID:
          return g_timer_info_table->header.el2_virt_timer_gsiv;
      case TIMER_INFO_NUM_PLATFORM_TIMERS:
          return g_timer_info_table->header.num_platform_timer;
      case TIMER_INFO_IS_PLATFORM_TIMER_SECURE:
          val_platform_timer_get_entry_index (instance, &block_num, &block_index);
          if (block_num != 0xFFFF)
              return ((g_timer_info_table->gt_info[block_num].flags[block_index] >> 16) & 1);
          break;
      case TIMER_INFO_SYS_CNTL_BASE:
          val_platform_timer_get_entry_index (instance, &block_num, &block_index);
          if (block_num != 0xFFFF)
              return g_timer_info_table->gt_info[block_num].block_cntl_base;
          break;
      case TIMER_INFO_SYS_CNT_BASE_N:
          val_platform_timer_get_entry_index (instance, &block_num, &block_index);
          if (block_num != 0xFFFF)
              return g_timer_info_table->gt_info[block_num].GtCntBase[block_index];
          break;
      case TIMER_INFO_SYS_INTID:
          val_platform_timer_get_entry_index (instance, &block_num, &block_index);
          if (block_num != 0xFFFF)
            return g_timer_info_table->gt_info[block_num].gsiv[block_index];
          break;
      case TIMER_INFO_PHY_EL1_FLAGS:
          return g_timer_info_table->header.ns_el1_timer_flag;
      case TIMER_INFO_VIR_EL1_FLAGS:
          return g_timer_info_table->header.virtual_timer_flag;
      case TIMER_INFO_PHY_EL2_FLAGS:
          return g_timer_info_table->header.el2_timer_flag;
      case TIMER_INFO_SYS_TIMER_STATUS:
          return g_timer_info_table->header.sys_timer_status;
    default:
      return 0;
  }
  return INVALID_INDEX;
}

void val_platform_timer_get_entry_index(uint64_t instance, uint32_t *block, uint32_t *index)
{
  if (instance > g_timer_info_table->header.num_platform_timer){
      *block = 0xFFFF;
      return;
  }

  *block = 0;
  *index = instance;
  while (instance > g_timer_info_table->gt_info[*block].timer_count){
      instance = instance - g_timer_info_table->gt_info[*block].timer_count;
      *index   = instance;
      *block   = *block + 1;
  }
}
/**
 * @brief   This API enables the Architecture timer whose register is given as the input parameter.
 *          1. Caller       -  VAL
 *          2. Prerequisite -  None
 * @param   reg  - system register of the ELx Arch timer.
 *
 * @return  None
 */
void ArmGenericTimerEnableTimer (ARM_ARCH_TIMER_REGS reg)
{
  uint64_t timer_ctrl_reg;

  timer_ctrl_reg = ArmArchTimerReadReg (reg);
  timer_ctrl_reg |= ARM_ARCH_TIMER_ENABLE;
  ArmArchTimerWriteReg (reg, &timer_ctrl_reg);
}

/**
 * @brief   This API disables the Architecture timer whose register is given as the input parameter.
 *          1. Caller       -  VAL
 *          2. Prerequisite -  None
 * @param   reg  - system register of the ELx Arch timer.
 *
 * @return  None
 */
void ArmGenericTimerDisableTimer (ARM_ARCH_TIMER_REGS reg)
{
  uint64_t timer_ctrl_reg;

  timer_ctrl_reg = ArmArchTimerReadReg (reg);
  timer_ctrl_reg &= ~ARM_ARCH_TIMER_ENABLE;
  ArmArchTimerWriteReg (reg, &timer_ctrl_reg);
}

/**
 * @brief   This API programs the el1 phy timer with the input timeout value.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   timeout - clock ticks after which an interrupt is generated.
 *
 * @return  None
 */
void val_timer_set_phy_el1(uint64_t timeout)
{

  if (timeout != 0) {
    ArmGenericTimerDisableTimer(CntpCtl);
    ArmArchTimerWriteReg(CntpTval, &timeout);
    ArmGenericTimerEnableTimer(CntpCtl);
  } else {
    ArmGenericTimerDisableTimer(CntpCtl);
 }
}

/**
 * @brief   This API programs the el1 Virtual timer with the input timeout value.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   timeout - clock ticks after which an interrupt is generated.
 *
 * @return  None
 */
void val_timer_set_vir_el1(uint64_t timeout)
{
  if (timeout != 0) {
    ArmGenericTimerDisableTimer(CntvCtl);
    ArmArchTimerWriteReg(CntvTval, &timeout);
    ArmGenericTimerEnableTimer(CntvCtl);
  } else {
    ArmGenericTimerDisableTimer(CntvCtl);
 }
}

/**
 * @brief   This API programs the el2 phy timer with the input timeout value.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   timeout - clock ticks after which an interrupt is generated.
 *
 * @return  None
 */
void val_timer_set_phy_el2(uint64_t timeout)
{

  if (timeout != 0) {
    ArmGenericTimerDisableTimer(CnthpCtl);
    ArmArchTimerWriteReg(CnthpTval, &timeout);
    ArmGenericTimerEnableTimer(CnthpCtl);
  } else {
    ArmGenericTimerDisableTimer(CnthpCtl);
 }
}

/**
 * @brief   This API programs the el2 Virt timer with the input timeout value.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  None
 * @param   timeout - clock ticks after which an interrupt is generated.
 *
 * @return  None
 */
void val_timer_set_vir_el2(uint64_t timeout)
{

  if (timeout != 0) {
    ArmGenericTimerDisableTimer(CnthvCtl);
    ArmArchTimerWriteReg(CnthvTval, &timeout);
    ArmGenericTimerEnableTimer(CnthvCtl);
  } else {
    ArmGenericTimerDisableTimer(CnthvCtl);
 }
}

/**
 * @brief   This API will call PAL layer to fill in the Timer information
 *          into the g_timer_info_table pointer.
 *          1. Caller       -  Application layer.
 *          2. Prerequisite -  Memory allocated and passed as argument.
 * @param   timer_info_table  pre-allocated memory pointer for timer_info
 * @return  Error if Input param is NULL
 */
void val_timer_create_info_table(uint64_t *timer_info_table)
{

  if (timer_info_table == NULL) {
      val_print(ACS_LOG_ERR, "\n        Input for Create Info table cannot be NULL");
      return;
  }

  g_timer_info_table = (timer_info_table_t *)timer_info_table;

  pal_timer_create_info_table(g_timer_info_table);

  /* UEFI or other EL1 software may have enabled the el1 physical timer.
     Disable the timer to prevent interrupts at un-expected times */

  val_timer_set_phy_el1(0);

  val_print(ACS_LOG_TEST, "\n        TIMER_INFO: Number of system timers  : %4d",
                                g_timer_info_table->header.num_platform_timer);
  val_pe_data_cache_clean_invalidate((uint64_t)&g_timer_info_table);

}

/**
 * @brief  This API will program and start the counter
 * @param cnt_base_n Timer base address
 * @param  timeout  timeout value for timer
 * @return  None
 */
void val_timer_set_system_timer(addr_t cnt_base_n, uint32_t timeout)
{
  /* Start the System timer */
  val_mmio_write(cnt_base_n + 0x28, timeout);

  /* enable System timer */
  val_mmio_write(cnt_base_n + 0x2C, 1);

}

acs_status_t val_configure_second_interrupt(uint32_t *index, uint64_t *int_id)
{
    return pal_configure_second_interrupt(index, int_id);
}

void val_generate_second_interrupt(uint32_t index, uint32_t timeout)
{
    pal_generate_second_interrupt(index, timeout);
}

void val_disable_second_interrupt(uint32_t index)
{
    pal_disable_second_interrupt(index);
}

/**
 * @brief  This API will stop the counter
 * @param cnt_base_n Timer base address
 * @return  None
 */
void val_timer_disable_system_timer(addr_t cnt_base_n)
{
  /* stop System timer */
  val_mmio_write(cnt_base_n + 0x2C, 0);
}

/**
 * @brief  This API will read CNTACR (from CNTCTLBase) to determine whether
 *         access permission from NS state is permitted
 * @param index  Timer index
 * @return  status
 */
uint32_t val_timer_skip_if_cntbase_access_not_allowed(uint64_t index)
{
  uint64_t cnt_ctl_base;
  uint32_t data;

  cnt_ctl_base = val_timer_get_info(TIMER_INFO_SYS_CNTL_BASE, index);
  if (cnt_ctl_base) {
      data = val_mmio_read(cnt_ctl_base + 0x40);
      if ((data & 0x1) == 0x1)
          return 0;
      else {
          data |= 0x1;
          val_mmio_write(cnt_ctl_base + 0x40, data);
          data = val_mmio_read(cnt_ctl_base + 0x40);
          if ((data & 0x1) == 1)
              return 0;
          else
              return SDEI_TEST_SKIP;
      }
  }
  else
      return SDEI_TEST_SKIP;
}

/**
 *  @brief   This function frees the allocated memory for timer info table
 *  @param   None
 *  @return  none
 */
void val_timer_free_info_table(void)
{
    pal_free_mem((uint64_t *)g_timer_info_table);
}
