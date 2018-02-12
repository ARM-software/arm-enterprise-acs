/** @file
 * Software Delegated Exception Interface (SDEI) API
 *
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
#include "val_sdei_interface.h"
#include "pal_interface.h"

event_info_table_t *g_event_info_table;

uint32_t val_acpi_present() {

    if (!pal_acpi_present()) {
        return -1;
    }

    return 0;
}

/**
 *  @brief   This function reads value stored in given register (x0 - x17), based on param_id.
 *
 *  @param query  Register value between 0 to 17
 *
 *  @return  status
 */
int32_t val_sdei_event_context(uint32_t query, uint64_t *result)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_CONTEXT, query, 0, 0, 0, 0,
                  result);
}

/**
 *  @brief   This function retrieves information about given event based on info parameter.
 *
 *  @param event  Event number
 *  @param info   Information requested
 *
 *  @return  status
 */
int32_t val_sdei_event_get_info(uint32_t event, uint32_t info, uint64_t *result)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_GET_INFO, event, info, 0,
                  0, 0, result);
}

/**
 *  @brief   This function marks the event complete, with completion status.
 *           It is called from event handler.
 *           Execution resumes in the context interrupted by the event.
 *
 *  @return  status
 */
int32_t val_sdei_event_complete(uint32_t status_code)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_COMPLETE, status_code, 0, 0,
            0, 0, NULL);
}

/**
 *  @brief   This function marks the event complete. Execution resumes from resume_addr.
 *
 *  @param return_address   Address in clent to resume the excution from
 *
 *  @return  status
 */
int32_t val_sdei_event_complete_and_resume(uint64_t return_address)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_COMPLETE_AND_RESUME, return_address, 0, 0,
            0, 0, NULL);
}

/**
 *  @brief   This function queries SDEI features implemented by the dispatcher.
 *
 *  @param feature   This argument specifies the feature that is queried.
 *
 *  @return  none
 */
int32_t val_sdei_features(uint32_t feature, uint64_t *num_slots)
{
    int32_t err;
    uint64_t result;

    err = pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_FEATURES, feature, 0, 0, 0,
                 0, &result);
    *num_slots = result;

    return err;
}

static int32_t val_event_create(struct sdei_event *event)
{
    int32_t err = 0;

    err = val_sdei_event_get_info(event->event_num, SDEI_EVENT_INFO_EV_PRIORITY,
                    &event->priority);
    if (err) {
        return err;
    }

    err = val_sdei_event_get_info(event->event_num, SDEI_EVENT_INFO_EV_TYPE,
                    &event->type);
    if (err) {
        return err;
    }

    return err;
}

/**
 *  @brief   This function returns event number based on the given type
 *
 *  @param type  This argument specifies whether event is shared or private
 *
 *  @return  On success it returns the event number
 */
uint32_t val_event_get(uint32_t type, uint32_t priority)
{
    struct sdei_event event;
    int err, i;
    uint32_t event_num = 0;
    event_info_t *event_info = &g_event_info_table->info[0];

    if (type == SDEI_EVENT_TYPE_PRIVATE) {
        err = val_sdei_interrupt_bind(PPI_INTR_NUM, &event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        PPI Interrupt bind failed with err %d", err);
        }
        return event_num;
    }
    else if (type == SDEI_EVENT_TYPE_SHARED) {
         err = val_sdei_interrupt_bind(SPI_INTR_NUM, &event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI Interrupt bind failed with err %d", err);
        }
        return event_num;
    }

    for (i = 0; i < g_event_info_table->num_events; i++) {
        event.event_num = event_info->number;
        if (priority == SDEI_EVENT_PRIORITY_ANY && type == SDEI_EVENT_TYPE_ANY)
            return event.event_num;
        err = val_event_create(&event);
        if (err)
            return 0;

        if ((priority == event.priority && type == event.type) ||
            (priority == SDEI_EVENT_PRIORITY_ANY && type == event.type) ||
            (priority == event.priority && type == SDEI_EVENT_TYPE_ANY))
            return event.event_num;
    }
    return 0;
}

/**
 *  @brief   This function returns the version of SDEI dispatcher.
 *
 *  @return  status
 */
int32_t val_sdei_get_version(uint64_t *version)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_VERSION, 0, 0, 0, 0, 0, version);
}

/**
 *  @brief   This function masks the PE from receiving any event.
 *
 *  @return  status
 */
int32_t val_sdei_mask(uint64_t *result)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_PE_MASK, 0, 0, 0, 0, 0, result);
}

/**
 *  @brief   This function unmasks the PE to receive events.
 *  @param   None
 *
 *  @return  status
 */
int32_t val_sdei_unmask(void)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_PE_UNMASK, 0, 0, 0, 0, 0, NULL);
}

/**
 *  @brief   This function resets SDEI private data.
 *
 *  @return  status
 */
int32_t val_sdei_private_reset(void *ignored)
{
    int32_t err = 0;

    err = pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_PRIVATE_RESET, 0, 0, 0, 0, 0,
                 NULL);
    return err;
}

/**
 *  @brief   This function resets shared SDEI data.
 *  @param   None
 *
 *  @return  status
 */
int32_t val_sdei_shared_reset(void)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_SHARED_RESET, 0, 0, 0, 0, 0,
                  NULL);
}

/**
 *  @brief   This function retrieves status of the event, as one or more of running,
 *           enabled or registered.
 *  @param event_num  Event number
 *
 *  @return  status
 */
int32_t val_sdei_event_status(uint32_t event_num, uint64_t *result)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_STATUS, event_num, 0, 0, 0,
                  0, result);
}

/**
 *  @brief   This function enables a given event so that registered handler is 
 *           called when event occurs.
 *  @param event_num  Event number
 *
 *  @return  status
 */
int32_t val_sdei_event_enable(uint32_t event_num)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_ENABLE, event_num, 0, 0, 0,
                  0, NULL);
}

/**
 *  @brief   This function disables a given event.
 *  @param event_num  Event number
 *
 *  @return  status
 */
uint32_t val_sdei_event_disable(uint32_t event_num)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_DISABLE, event_num, 0, 0,
                  0, 0, NULL);
}

/**
 *  @brief   This function binds the given interrupt to given event. 
 *  @param irq_num  Interrupt number
 *
 *  @return  status
 */
int32_t val_sdei_interrupt_bind(uint32_t irq_num, uint32_t *event_num)
{
    int32_t err;
    uint64_t result;

    err = pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_INTERRUPT_BIND, irq_num, 0, 0, 0,
                 0, &result);
    *event_num = result;

    return err;
}

/**
 *  @brief   This function releases the interrupt bound to given input. 
 *  @param event_num  Event number
 *
 *  @return  status
 */
int32_t val_sdei_interrupt_release(uint32_t event_num)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_INTERRUPT_RELEASE, event_num, 0,
                  0, 0, 0, NULL);
}

/**
 *  @brief   This function unregisters the given event with the dispatcher.
 *  @param event_num  Event number
 *
 *  @return  status
 */
int32_t val_sdei_event_unregister(uint32_t event_num)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_UNREGISTER, event_num, 0,
                  0, 0, 0, NULL);
}

/**
 *  @brief   This function signals software event to given target PE. 
 *  @param event_num  Event number
 *  @param affinity   Target PE affinity
 *
 *  @return  status
 */
uint32_t val_sdei_event_signal(uint32_t event_num, uint64_t affinity)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_SIGNAL, event_num, affinity,
                                        0, 0, 0, NULL);
}

/**
 *  @brief   This function registers a given event with the dispatcher.
 *  @param event_num    Event number
 *  @param entry_point  Entry point address at ELc for the event handler
 *  @param arg          User defined argument passed to entry point routine.
 *  @param flags        Routing mode flags
 *  @param affinity     Target PE affinity
 *
 *  @return  status
 */
uint32_t val_sdei_event_register(uint32_t event_num, uint64_t entry_point,
                   void *arg, uint64_t flags, uint64_t affinity)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_REGISTER, event_num,
                  (uint64_t)entry_point, (uint64_t)arg,
                  flags, affinity, NULL);
}

static uint32_t sdei_event_routing_set(uint32_t event_num, uint64_t routing_mode,
                      uint64_t affinity)
{
    return pal_invoke_sdei_fn(SDEI_1_0_FN_SDEI_EVENT_ROUTING_SET, event_num,
                  routing_mode, affinity, 0, 0, NULL);
}

/**
 *  @brief   This function changes the routing information of given event. 
 *  @param event_num  Event number
 *  @param directed   routing mode RM_ANY or RM_PE
 *  @param cpu_id     Target PE index
 *
 *  @return  status
 */
uint32_t val_sdei_event_routing_set(uint32_t event_num, bool directed, int32_t cpu_id)
{
    uint32_t err;
    uint64_t affinity;
    uint32_t routing_mode;

    if (directed) {
        routing_mode = SDEI_EVENT_REGISTER_RM_PE;
        affinity = val_pe_get_mpid_index(cpu_id);
        if (affinity == INVALID_INDEX) {
            val_print(ACS_LOG_DEBUG, "\n        Failed to determine affinity for CPU %u", cpu_id);
        }
    } else {
        routing_mode = SDEI_EVENT_REGISTER_RM_ANY;
        affinity = 0;
    }
    err = sdei_event_routing_set(event_num, routing_mode,
                     affinity);
    if (err)
        val_print(ACS_LOG_DEBUG, "\n        Failed to set routing mode");

    return err;
}

/**
 *  @brief   This function retrieves routing information about given event
 *  @param event_num  Event number
 *
 *  @return  status
 */
uint32_t val_sdei_event_routing_get(uint32_t event_num, bool *directed, int32_t *cpu_id)
{
    uint32_t err;
    uint64_t info;

    err = val_sdei_event_get_info(event_num,
                    SDEI_EVENT_INFO_EV_ROUTING_MODE,
                    &info);
    if (err) {
        val_print(ACS_LOG_DEBUG, "\n        Failed to query routing mode");
        return err;
    }
    *directed = (info == SDEI_EVENT_REGISTER_RM_PE);

    err = val_sdei_event_get_info(event_num, SDEI_EVENT_INFO_EV_ROUTING_AFF,
                    &info);
    if (err) {
        val_print(ACS_LOG_DEBUG, "\n        Failed to query routing affinity");
        return err;
    }
    *cpu_id = val_pe_get_index_mpid(info);
    /* Don't print a warning if the affinity is bogus for RM_ANY */
    if (*cpu_id == INVALID_INDEX)
        val_print(ACS_LOG_ERR, "\n        Failed to convert firmware description for affinity %llx",
                                                                                            info);

    return err;
}

/**
 *  @brief   This function creates a table of information on various SDEI events in the system.
 *  @param table   pre-allocated memory pointer for event_info
 *
 *  @return  status
 */
acs_status_t val_sdei_create_event_info_table(uint64_t *table)
{
    int status;
    if (table == NULL)
        return ACS_ERROR;
    g_event_info_table = (event_info_table_t*)table;

    status = pal_sdei_create_event_info_table(g_event_info_table);
    if (!status) {
        val_print(ACS_LOG_TEST, "\n        EVT_INFO: Number of hardware error events  : %4d",
                                                                g_event_info_table->num_events);
    }
    else {
        val_print(ACS_LOG_ERR, "\n        EVT_INFO: No hardware error event found");
    }

    val_pe_data_cache_clean_invalidate((uint64_t)&g_event_info_table);
    return ACS_SUCCESS;
}

uint32_t val_event_get_hest_info(uint32_t type, uint64_t *result)
{
    switch (type)
    {
        case HEST_FOUND:
            *result = g_event_info_table->hest_found;
            break;

        case NUM_GHES:
            *result = g_event_info_table->num_ghes_notify;
            break;

        case NUM_EVENTS:
            *result = g_event_info_table->num_events;
            break;

        case HEST_NOTIFY:
            *result = (uint64_t)&g_event_info_table->info;
            break;

        default:
            *result = 0;
            val_print(ACS_LOG_ERR, "\n        HEST type is not valid");
            return 1;
    }
    return 0;
}

/**
 *  @brief   This function initialises the SDEI, unmask and resets the all PEs
 *  @param   None
 *  @return  status
 */
int32_t val_sdei_initialization(void)
{
    uint32_t err;
    uint64_t ver = 0;
    uint32_t conduit;

    if (!pal_acpi_present()) {
        val_print(ACS_LOG_WARN, "\n        SDEI entry not found in ACPI table");
    }

    conduit = pal_conduit_get();
    if (!conduit)
        return -1;

    err = val_sdei_get_version(&ver);
    if (err == SDEI_STATUS_NOT_SUPPORTED)
        val_print(ACS_LOG_ERR, "\n        advertised but not implemented in platform firmware");
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Failed to get SDEI version: %d", err);
        return err;
    }

    val_print(ACS_LOG_TEST, "\n        SDEIv%d.%d (0x%x) detected in firmware.",
        (uint32_t)SDEI_VERSION_MAJOR(ver), (uint32_t)SDEI_VERSION_MINOR(ver),
        (uint32_t)SDEI_VERSION_VENDOR(ver));

    if (SDEI_VERSION_MAJOR(ver) != 1) {
        val_print(ACS_LOG_WARN, "\n        Conflicting SDEI version detected.");
        return SDEI_STATUS_INVALID;
    }

    val_pe_execute_on_all(val_sdei_unmask, 0);
    val_pe_execute_on_all(val_sdei_private_reset, 0);
    val_sdei_shared_reset();

    return 0;
}

/**
 *  @brief   This function frees table of information on various SDEI events in the system. 
 *  @param   None
 *
 *  @return  none
 */
void val_event_free_info_table(void)
{
    pal_free_mem((uint64_t *)g_event_info_table);
}
