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

#ifndef __MPAM_ACS_MPAM_H__
#define __MPAM_ACS_MPAM_H__

#define HARDLIMIT_DIS 0x0
#define HARDLIMIT_EN 0x1
#define CPOR_BITMAP_DEF_VAL 0xFFFF


uint8_t val_cache_supports_cpor(uint32_t node_index);
uint8_t val_cache_supports_ccap(uint32_t node_index);
uint8_t val_cache_supports_csumon(uint32_t node_index);
uint16_t val_cache_cpbm_width(uint32_t node_index);
uint8_t val_cache_cmax_width(uint32_t node_index);
uint16_t val_cache_mon_count(uint32_t node_index);
void val_cache_configure_cpor(uint32_t node_index, uint16_t partid, uint32_t cpbm_percentage);
void val_cache_configure_ccap(uint32_t node_index, uint16_t partid, uint8_t hardlim, uint32_t ccap_percentage);
uint32_t val_cache_get_size(uint32_t node_index);

uint32_t testc001_entry();
uint32_t testc002_entry();
uint32_t testc003_entry();
uint32_t testc004_entry();
uint32_t testc005_entry();
uint32_t testc006_entry();

#endif

