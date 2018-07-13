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

#ifndef __MPAM_CACHE_MONITOR_H__
#define __MPAM_CACHE_MONITOR_H__


#define CPOR_BITMAP_DEF_VAL 0xFFFF

void val_csumon_config_monitor(uint32_t node_index, uint16_t partid, uint8_t pmg, uint16_t mon_sel);
void val_csumon_restore_ctlreg(uint32_t node_index, uint16_t mon_sel);
uint16_t val_csumon_monitor_count(uint32_t node_index);
uint32_t val_csumon_storage_value(uint32_t node_index, uint16_t mon_sel);
void val_csumon_reset_monitor(uint32_t node_index, uint16_t mon_sel);

uint32_t testm001_entry();
uint32_t testm002_entry();
uint32_t testm003_entry();

#endif

