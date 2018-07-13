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

#ifndef __MPAM_ACS_NODE_INFRA_H__
#define __MPAM_ACS_NODE_INFRA_H__

#define DEFAULT_PARTID 0
#define DEFAULT_PARTID_MAX 65535 //(2^16 - 1)
#define DEFAULT_PMG 0
#define DEFAULT_PMG_MAX 255 //(2^8 - 1)
#define DEFAULT_MONCNT 0
#define DEFAULT_MONCNT_MAX 65535 //(2^16 - 1)
#define POR_BUF_SIZE 1024
#define PMGOR_BUF_SIZE 1024

#define COUNT_BW_RD 1
#define COUNT_BW_WR 2
#define COUNT_BW_RW 3

#define ONE_MB (1024*1024)
#define TWO_MB (2*1024*1024)
#define MBWU_VALUE_MAX 0xEFFFFFFF

#if MPAM_SIMULATION_FVP
#define MPAMIDR_EL1_FVP ((50UL << MPAMIDR_PMG_MAX_SHIFT) | (90UL << MPAMIDR_PARTID_MAX_SHIFT))
#endif

typedef enum {
    MPAMIDR_SYSREG = 0,
    MPAM1_SYSREG,
    MPAM2_SYSREG,
}sysreg_mpam_t;

uint64_t val_sysreg_read(sysreg_mpam_t regid);
void     val_sysreg_write(sysreg_mpam_t regid, uint64_t data);

void     val_mpam_free_info_table();
uint32_t val_node_get_total(uint8_t node_type);
addr_t   val_node_hwreg_base(uint8_t node_type, uint32_t node_index);
uint16_t val_node_get_partid(uint8_t node_type, uint32_t node_index);
uint8_t  val_node_get_pmg(uint8_t node_type, uint32_t node_index);
uint32_t val_node_get_error_intrnum(uint8_t node_type, uint32_t node_index);
uint32_t val_node_get_error_intrtype(uint8_t node_type, uint32_t node_index);
uint32_t val_node_get_oflow_intrnum(uint8_t node_type, uint32_t node_index);
uint32_t val_node_get_oflow_intrtype(uint8_t node_type, uint32_t node_index);
void     val_node_trigger_intr(uint8_t node_type, uint32_t node_index);
void     val_node_clear_intr(uint8_t node_type, uint32_t node_index);
void     val_node_restore_ecr(uint8_t node_type, uint32_t node_index);
void     val_node_generate_psr_error(uint8_t node_type, uint32_t node_index);
uint8_t  val_node_supports_mon(uint8_t node_type, uint32_t node_index);
void     val_node_generate_msmon_oflow_error(uint32_t node_index, uint16_t mon_count);
void     val_node_generate_msmon_config_error(uint8_t node_type, uint32_t node_index, uint16_t mon_count);
void     val_node_generate_msr_error(uint8_t node_type, uint32_t node_index, uint16_t mon_count);
uint32_t val_node_generate_por_error(uint8_t node_type, uint32_t node_index);
uint32_t val_node_generate_pmgor_error(uint8_t node_type, uint32_t node_index);

void     arm64_issue_dmb(void);
void     arm64_issue_dsb(void);
void     arm64_issue_isb(void);
uint64_t sysreg_read_mpamidr(void);
uint64_t sysreg_read_mpam1(void);
uint64_t sysreg_read_mpam2(void);
void     sysreg_write_mpam2(uint64_t value);

#endif

