uint64_t ArmReadMpidr(void);
uint64_t ArmReadIdPfr0(void);
uint64_t AA64ReadCurrentEL(void);
uint64_t ArmReadDaif(void);
uint64_t ArmReadSPselPState(void);
uint64_t ArmReadElrEl2(void);
uint64_t ArmReadSpsrEl2(void);
uint64_t ArmReadElrEl1(void);
uint64_t ArmReadSpsrEl1(void);

acs_status_t val_is_el3_enabled(void);
acs_status_t val_is_el2_enabled(void);
uint64_t val_pe_get_mpid(void);
uint64_t val_pe_get_mpid_index(uint32_t index);
uint32_t val_pe_get_index_mpid(uint64_t mpid);
