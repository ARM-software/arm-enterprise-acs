/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

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

#ifndef __VAL_SDEI_INTERFACE_H
#define __VAL_SDEI_INTERFACE_H

#include "val_specification.h"

/* kvm needs to know if hvcs are for SDEI or PSCI */
#define IS_SDEI_CALL(x)        ((x & SDEI_1_0_MASK) == SDEI_1_0_FN_BASE)

/* Values for sdei_exit_mode */
#define SDEI_EXIT_HVC  0
#define SDEI_EXIT_SMC  1

#define HEST_FOUND     1
#define NUM_GHES       2
#define NUM_EVENTS     3
#define HEST_NOTIFY    4

struct sdei_event {
    uint32_t         event_num;
    uint64_t         type;
    uint64_t         priority;
    bool        is_bound_irq;
};

/* Arch code should override this to set the entry point from firmware... */
#ifndef sdei_arch_get_entry_point
#define sdei_arch_get_entry_point(conduit)    (0)
#endif

typedef int (sdei_event_callback)(uint32_t event, void *arg);

int val_sdei_initialization(void);

uint32_t val_sdei_event_register(uint32_t event_num, uint64_t entry_point,
                   void *arg, uint64_t flags, uint64_t affinity);
int32_t val_sdei_event_enable(uint32_t event_num);
uint32_t val_sdei_event_disable(uint32_t event_num);
uint32_t val_event_get(uint32_t type, uint32_t priority);
uint32_t val_sdei_event_signal(uint32_t event_num, uint64_t affinity);
uint32_t val_acpi_present(void);
int32_t val_sdei_event_unregister(uint32_t event_num);

uint32_t val_sdei_event_routing_set(uint32_t event_num, bool directed, int to_cpu);
uint32_t val_sdei_event_routing_get(uint32_t event_num, bool *directed, int *to_cpu);

int32_t val_sdei_interrupt_bind(uint32_t irq_num, uint32_t *event_num);
int32_t val_sdei_interrupt_release(uint32_t event_num);
int32_t val_sdei_features(uint32_t feature, uint64_t *num_slots);
int32_t val_sdei_get_version(uint64_t *version);

int32_t val_sdei_event_status(uint32_t event_num, uint64_t *result);
int32_t val_sdei_event_get_info(uint32_t event, uint32_t info, uint64_t *result);
int32_t val_sdei_private_reset(void *ignored);
int32_t val_sdei_shared_reset(void);

/* For use by arch code when CPU hotplug notifiers are not appropriate. */
int32_t val_sdei_mask(uint64_t *result);
int32_t val_sdei_unmask(void);

/* arch code may use this to retrieve the extra registers. */
int32_t val_sdei_event_context(uint32_t query, uint64_t *result);
int32_t val_sdei_event_complete(uint32_t status_code);
int32_t val_sdei_event_complete_and_resume(uint64_t return_address);

uint32_t val_event_get_hest_info(uint32_t type, uint64_t *result);
#endif /* __VAL_SDEI_INTERFACE_H */
