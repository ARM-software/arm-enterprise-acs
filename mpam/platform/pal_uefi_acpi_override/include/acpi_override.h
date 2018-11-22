
#ifndef __ACPI_OVERRIDE__
#define __ACPI_OVERRIDE__

#include <Uefi.h>
#include <Library/UefiLib.h>

#define MAX_CACHE_NODES 256
#define MAX_DDR_NODES   16

/*
 * @brief   Cache Type Structure Cache Attributes
 */
typedef struct {
    UINT8     alloc_type;
    UINT8     cache_type;
    UINT8     write_policy;
    UINT8     reserved;
} CACHE_NODE_ATTR_OVERRIDE;

/*
 * @brief   Cache Node Hierarchy Information
 *             Scope Index =   Provides more information for node scope
 *              = Don't care if node_scope == SYSTEM
 *              = (Aff3 <<16) | (Aff2 << 8) | (Aff1 << 0), if node_scope = CLUSTER
 *              = Aff0, if node_scope = PE
 */
typedef struct {
    UINT32    scope_index;
    UINT32    node_scope;  //Indicates if the cache node scope is PE, CLUSTER or SYSTEM
} CACHE_NODE_INFO_OVERRIDE;

typedef struct {
    UINT32        error_intr_num;
    UINT32        overflow_intr_num;
    UINT32        error_intr_type;  //0 for edge and 1 for level
    UINT32        overflow_intr_type;
} INTR_INFO_OVERRIDE;

typedef struct {
    UINT64        mpidr;
    UINT32        line_size;
    UINT32        size;
    UINT64        neighbours[4];
    CACHE_NODE_INFO_OVERRIDE info;
    CACHE_NODE_ATTR_OVERRIDE attributes;
    UINT64          hwreg_base_addr;
    UINT32        not_ready_max_us;
    INTR_INFO_OVERRIDE       intr_info;
} mpam_cache_node_cfg;


/*
 * @brief   Mpam Dram Node Entry
 */
typedef struct {
    UINT32    proximity_domain;
    UINT64    base_address;
    UINT64    length;
    UINT32    flags;
    UINT64      hwreg_base_addr;
    UINT32    not_ready_max_us;
    INTR_INFO_OVERRIDE   intr_info;
} memory_node_entry;




typedef struct {
  UINT32              num_cache_nodes;
  mpam_cache_node_cfg cnode[MAX_CACHE_NODES];
  UINT32              num_memory_nodes;
  memory_node_entry  mnode[MAX_DDR_NODES];
} platform_mpam_cfg;

typedef struct {
  mpam_cache_node_cfg* cache;
  struct PrivatePeCaches* next;
} PrivatePeCaches;

#endif
