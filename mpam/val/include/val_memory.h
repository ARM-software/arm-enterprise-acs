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

#ifndef __MPAM_ACS_MPAM_MEMORY_H__
#define __MPAM_ACS_MPAM_MEMORY_H__

#define HARDLIMIT_DIS 0x0
#define HARDLIMIT_EN 0x1
#define MBWPOR_BITMAP_DEF_VAL 0xFFFFFFFF

uint8_t val_memory_supports_part(uint32_t node_index);
uint8_t val_memory_supports_mbwmin(uint32_t node_index);
uint8_t val_memory_supports_mbwmax(uint32_t node_index);
uint8_t val_memory_supports_mbwpbm(uint32_t node_index);
uint16_t val_memory_mbwmin_width(uint32_t node_index);
uint16_t val_memory_mbwmax_width(uint32_t node_index);
uint16_t val_memory_mbwpbm_width(uint32_t node_index);
uint8_t val_memory_supports_mbwumon(uint32_t node_index);
uint16_t val_memory_mon_count(uint32_t node_index);
void val_memory_configure_mbwmin(uint32_t node_index, uint16_t partid, uint32_t mbwmin_percentage);
void val_memory_configure_mbwmax(uint32_t node_index, uint16_t partid, uint8_t hardlim, uint32_t mbwmax_percentage);
void val_memory_configure_mbwpbm(uint32_t node_index, uint16_t partid, uint32_t mbwpbm_percentage);
uint64_t val_memory_get_base(uint32_t node_index);
uint64_t val_memory_get_size(uint32_t node_index);

uint32_t testd001_entry();
uint32_t testd002_entry();
uint32_t testd003_entry();

#endif

