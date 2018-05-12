# SBSA ACS Testcase checklist
Please refer https://github.com/ARM-software/sbsa-acs/blob/master/docs/testcase-checklist.md


# SBBR ACS Testcase checklist

| Component | Test assertion | SCT/FWTS | SBBR Section |
| :--- | ---: | ---: | ---: |
| UEFI - BOOT SERVICES | Memory map will be identified. | SCT | 3.4.1 |
| UEFI - BOOT SERVICES | Default RAM attribute must be EFI_MEMORY_WB. | SCT | 3.4.1 |
| UEFI - BOOT SERVICES | EFI_ACPI_60_TABLE_GUID must be present in system configuration table. | SCT, FWTS | 3.4.4 |
| UEFI - BOOT SERVICES | SMBIOS3_TABLE_GUID must be present in system configuration table. | SCT, FWTS | 3.4.4 |
| UEFI - BOOT SERVICES | SMBIOS_TABLE_GUID must be present in system configuration table. | SCT, FWTS | 3.4.4 |
| UEFI - BOOT SERVICES | EFI_CREATE_EVENT tests | SCT | Appendix A  5.1.1.1.* |
| UEFI - BOOT SERVICES | EFI_CLOSE_EVENT tests | SCT | Appendix A  5.1.1.2.* |
| UEFI - BOOT SERVICES | EFI_SIGNAL_EVENT tests | SCT | Appendix A  5.1.1.3.* |
| UEFI - BOOT SERVICES | EFI_WAIT_FOR_EVENT tests | SCT | Appendix A  5.1.1.4.* |
| UEFI - BOOT SERVICES | EFI_SET_TIMER tests | SCT | Appendix A  5.1.1.6.* |
| UEFI - BOOT SERVICES | EFI_RAISE_TPL tests | SCT | Appendix A  5.1.1.7.* |
| UEFI - BOOT SERVICES | EFI_RESTORE_TPL tests | SCT | Appendix A  5.1.1.8.* |
| UEFI - BOOT SERVICES | EFI_CREATE_EVENT_EX tests | SCT | Appendix A  5.1.1.9.* |
| UEFI - BOOT SERVICES | EFI_ALLOCATE_PAGES tests | SCT | Appendix A  5.1.2.1.* |
| UEFI - BOOT SERVICES | EFI_FREE_PAGES tests | SCT | Appendix A  5.1.2.2.* |
| UEFI - BOOT SERVICES | EFI_GET_MEMORY_MAP tests | SCT | Appendix A  5.1.2.3.* |
| UEFI - BOOT SERVICES | EFI_ALLOCATE_POOL tests | SCT | Appendix A  5.1.2.4.* |
| UEFI - BOOT SERVICES | EFI_INSTALL_PROTOCOL_INTERFACE tests | SCT | Appendix A  5.1.3.1.* |
| UEFI - BOOT SERVICES | EFI_UNINSTALL_PROTOCL_INTERFACE tests | SCT | Appendix A  5.1.3.2.* |
| UEFI - BOOT SERVICES | EFI_REINSTALL_PROTOCOL_INTERFACE tests |  SCT | Appendix A  5.1.3.3.* |
| UEFI - BOOT SERVICES | EFI_REGISTER_PROTOCOL_NOTIFY tests | SCT | Appendix A  5.1.3.4.* |
| UEFI - BOOT SERVICES | EFI_LOCATE_HANDLE tests | SCT | Appendix A  5.1.3.5.* |
| UEFI - BOOT SERVICES | EFI_HANDLE_PROTOCOL tests | SCT | Appendix A  5.1.3.6.* |
| UEFI - BOOT SERVICES | EFI_LOCATE_DEVICE_PATH tests | SCT | Appendix A  5.1.3.7.* |
| UEFI - BOOT SERVICES | EFI_OPEN_PROTOCOL tests | SCT | Appendix A  5.1.3.8.* |
| UEFI - BOOT SERVICES | EFI_CLOSE_PROTOCOL tests | SCT | Appendix A  5.1.3.9.* |
| UEFI - BOOT SERVICES | EFI_OPEN_PROTOCOL_INFORMATION tests | SCT Appendix A |  5.1.3.10.* |
| UEFI - BOOT SERVICES | EFI_CONNECT_CONTROLLER tests | SCT | Appendix A  5.1.3.11.* |
| UEFI - BOOT SERVICES | EFI_DISCONNECT_CONTROLLER tests | SCT | Appendix A  5.1.3.12.* |
| UEFI - BOOT SERVICES | EFI_PROTOCOLS_PER_HANDLE tests | SCT | Appendix A  5.1.3.13.* |
| UEFI - BOOT SERVICES | EFI_LOCATE_HANDLE_BUFFER tests | SCT | Appendix A  5.1.3.14.* |
| UEFI - BOOT SERVICES | EFI_LOCATE_PROTOCOL tests | SCT | Appendix A  5.1.3.15.* |
| UEFI - BOOT SERVICES | EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES tests | SCT | Appendix A  5.1.3.16.* |
| UEFI - BOOT SERVICES | EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES tests | SCT | Appendix A  5.1.3.17.* |
| UEFI - BOOT SERVICES | EFI_IMAGE_LOAD tests | SCT | Appendix A  5.1.4.1.* |
| UEFI - BOOT SERVICES | EFI_IMAGE_START tests | SCT | Appendix A  5.1.4.2.* |
| UEFI - BOOT SERVICES | EFI_IMAGE_UNLOAD tests | SCT | Appendix A  5.1.4.3.* |
| UEFI - BOOT SERVICES | EFI_EXIT tests | SCT | Appendix A  5.1.4.5.* |
| UEFI - BOOT SERVICES | EFI_EXIT_BOOT_SERVICES tests | SCT | Appendix A  5.1.4.6.* |
| UEFI - BOOT SERVICES | EFI_SET_WATCHDOG_TIMER tests | SCT | Appendix A  5.1.5.1.* |
| UEFI - BOOT SERVICES | EFI_STALL tests | SCT | Appendix A  5.1.5.2.* |
| UEFI - BOOT SERVICES | EFI_COPY_MEM tests | SCT | Appendix A  5.1.5.3.* |
| UEFI - BOOT SERVICES | EFI_SET_MEM tests | SCT | Appendix A  5.1.5.4.* |
| UEFI - BOOT SERVICES | EFI_GET_NEXT_MONOTONIC_COUNT tests | SCT | Appendix A  5.1.5.5.* |
| UEFI - BOOT SERVICES | EFI_INSTALL_CONFIGURATION_TABLE tests | SCT | Appendix A  5.1.5.6.* |
| UEFI - BOOT SERVICES | EFI_CALCULATE_CRC32 tests | SCT | Appendix A  5.1.5.7.* |
| UEFI - BOOT SERVICES | All mapped 4K memory pages must have identical page attributes within the same physical 64K page. | SCT | 3.5.2 |
| UEFI - BOOT SERVICES | EFI_GET_TIME / GetTime() will be provided. | SCT | 3.5.3 |
| UEFI - BOOT SERVICES | EFI_SET_TIME / SetTime() will be provided. | SCT | 3.5.3 |
| UEFI - BOOT SERVICES | ResetSystem() must be provided. | SCT | 3.5.4 |
| UEFI - BOOT SERVICES | Persitent variable services must be provided. | SCT | 3.5.5, 5.2.1.3.23-26 |
| UEFI - BOOT SERVICES | EFI_GET_VARIABLE tests | SCT | Appendix B  5.2.1.1.* |
| UEFI - BOOT SERVICES | EFI_GET_NEXT_VARIABLE_NAME tests | SCT | Appendix B  5.2.1.2.* |
| UEFI - BOOT SERVICES | EFI_SET_VARIABLE tests | SCT | Appendix B  5.2.1.3.* |
| UEFI - BOOT SERVICES | EFI_QUERY_VARIABLE_INFO tests | SCT | Appendix B  5.2.1.4.* |
| UEFI - BOOT SERVICES | EFI_GET_TIME tests | SCT | Appendix B  5.2.2.1.* |
| UEFI - BOOT SERVICES | EFI_SET_TIME tests | SCT | Appendix B  5.2.2.2.* |
| UEFI - BOOT SERVICES | EFI_GET_WAKEUP_TIME tests | SCT | Appendix B  5.2.2.3.* |
| UEFI - BOOT SERVICES | EFI_SET_WAKEUP_TIME tests | SCT | Appendix B  5.2.2.4.* |
| UEFI - BOOT SERVICES | EFI_RESET_SYSTEM | SCT | Appendix B  5.2.4.1.* |
| UEFI - BOOT SERVICES | EFI_UPDATE_CAPSULE tests | SCT | Appendix B  5.2.4.2.* |
| UEFI - BOOT SERVICES | EFI_QUERY_CAPSULE_CAPABILITIES tests | SCT | Appendix B  5.2.4.3.* |
| UEFI - BOOT SERVICES | EFI_SET_VIRTUAL_ADDRESS_MAP tests | FWTS | Appendix B |
| UEFI - BOOT SERVICES | EFI_CONVERT_POINTER tests | FWTS | Appendix B |
| UEFI - PROTOCOLS | EFI_LOADED_IMAGE_PROTOCOL | SCT | Appendix C  5.3.1.1.* |
| UEFI - PROTOCOLS | EFI_DEVICE_PATH_PROTOCOL | SCT | Appendix C  5.4.1-2.*.* |
| UEFI - PROTOCOLS | EFI_DECOMPRESS_PROTOCOL | SCT | Appendix C  5.13.1.1-2.* |
| UEFI - PROTOCOLS | EFI_DEVICE_PATH_UTILITIES_PROTOCOL | SCT | Appendix C  5.4.3-4.*.* |
| UEFI - PROTOCOLS | EFI_LOAD_FILE_PROTOCOL | SCT | Appendix C |
| UEFI - PROTOCOLS | EFI_LOAD_FILE2_PROTOCOL | SCT | Appendix C |
| UEFI - PROTOCOLS | EFI_SIMPLE_FILE_SYSTEM_PROTOCOL | SCT | Appendix C  5.7.2.1.* |
| UEFI - PROTOCOLS | EFI_FILE_PROTOCOL | SCT | Appendix C  5.7.3.1-15.* |
| UEFI - PROTOCOLS | EFI_SIMPLE_TEXT_INPUT_PROTOCOL | SCT | Appendix C  5.6.1.1.* |
| UEFI - PROTOCOLS | EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL | SCT | Appendix C  5.6.7.1-5.* |
| UEFI - PROTOCOLS | EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL | SCT | Appendix C  5.6.2.1.* |
| UEFI - PROTOCOLS | EFI_HII_DATABASE_PROTOCOL | SCT | Appendix C  5.18.4.1-11.* |
| UEFI - PROTOCOLS | EFI_HII_STRING_PROTOCOL | SCT | Appendix C  5.18.2.1-5.* |
| UEFI - PROTOCOLS | EFI_HII_CONFIG_ROUTING_PROTOCOL | SCT | Appendix C  5.18.5.1-6.* |
| UEFI - PROTOCOLS | EFI_HII_CONFIG_ACCESS_PROTOCOL | SCT | Appendix C  5.18.6.1-2.* |
| UEFI - PROTOCOLS | EFI_SIMPLE_NETWORK_PROTOCOL | SCT | Appendix D  5.11.1.1-13.* |
| UEFI - PROTOCOLS | EFI_MANAGED_NETWORK_PROTOCOL | SCT | Appendix D  5.23.1.1-10.* |
| UEFI - PROTOCOLS | EFI_MANAGED_NETWORK_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.23.1.1-10.* |
| UEFI - PROTOCOLS | EFI_PXE_BASE_CODE_PROTOCOL | SCT | Appendix D  5.11.2.1-12.* |
| UEFI - PROTOCOLS | EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL | SCT | Appendix D  5.11.2.1-12.* |
| UEFI - PROTOCOLS | EFI_MTFTP4_PROTOCOL | SCT | Appendix D  5.26.2.1-10.* |
| UEFI - PROTOCOLS | EFI_MTFTP6_PROTOCOL | SCT | Appendix D  5.26.4.1-10.* |
| UEFI - PROTOCOLS | EFI_ARP_PROTOCOL | SCT | Appendix D  5.24.1.1-10.* |
| UEFI - PROTOCOLS | EFI_ARP_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.24.1.1-10.* |
| UEFI - PROTOCOLS | EFI_DHCP4_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.24.2.1-11.* |
| UEFI - PROTOCOLS | EFI_DHCP4_PROTOCOL | SCT | Appendix D  5.24.2.1-11.* |
| UEFI - PROTOCOLS | EFI_TCP4_PROTOCOL | SCT | Appendix D  5.25.1.1-25.* |
| UEFI - PROTOCOLS | EFI_TCP4_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.25.1.1-25.* |
| UEFI - PROTOCOLS | EFI_IP4_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.25.2.1-10.* |
| UEFI - PROTOCOLS | EFI_IP4_CONFIG_PROTOCOL | SCT | Appendix D  5.25.2.1-10.* |
| UEFI - PROTOCOLS | EFI_UDP4_PROTOCOL | SCT | Appendix D  5.26.1.1-10.* |
| UEFI - PROTOCOLS | EFI_UDP4_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.26.1.1-10.* |
| UEFI - PROTOCOLS | EFI_DHCP6_PROTOCOL | SCT | Appendix D  5.24.3.1-11.* |
| UEFI - PROTOCOLS | EFI_DHCP6_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.24.3.1-11.* |
| UEFI - PROTOCOLS | EFI_TCP6_PROTOCOL | SCT | Appendix D  5.25.4.1-8.* |
| UEFI - PROTOCOLS | EFI_IP6_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.25.5.1-11.* |
| UEFI - PROTOCOLS | EFI_IP6_CONFIG_PROTOCOL | SCT | Appendix D  5.25.5.1-11.* |
| UEFI - PROTOCOLS | EFI_UDP6_PROTOCOL | SCT | Appendix D  5.26.3.1-8.* |
| UEFI - PROTOCOLS | EFI_UDP6_SERVICE_BINDING_PROTOCOL | SCT | Appendix D  5.26.3.1-8.* |
| UEFI - PROTOCOLS | EFI_VLAN_CONFIG_PROTOCOL | SCT | Appendix D  5.27.1.1-3.* |
| UEFI - PROTOCOLS | EFI_ISCSI_INITIATOR_NAME_PROTOCOL | SCT | Appendix D  5.17.1-2.1.* |
| UEFI - PROTOCOLS | EFI_GRAPHICS_OUTPUT_PROTOCOL | SCT | 3.3.5 |
| UEFI - PROTOCOLS | EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL | SCT | 3.3.6 |
| UEFI - PROTOCOLS | EFI_PCI_IO_PROTOCOL | SCT | 3.3.6 |
| ACPI | ACPI must be compliant with version 6.2 or later. | FWTS | 4.1 |
| ACPI | Implement the HW-Reduced ACPI model. | FWTS | 4.1 |
| ACPI | Warn if legacy HW interfaces are supported as it should be HW reduced platform. | FWTS | 4.1/4.2.1.3 |
| ACPI | Provide GPIO signalled events for runtime notifications to OSPM. | FWTS | 4.1 |
| ACPI | RSDP must be implemented where RsdtAddress field must be null (zero) and the XsdtAddresss MUST be a valid, non-null, 64-bit value. | FWTS | 4.2.1.1 |
| ACPI | XSDT must be present, pointed at by XsdrAddress, and contain valid pointers to all other ACPI tables mandated by this spec. | FWTS | 4.2.1.2 |
| ACPI | It is recommended that server profile is selected. | FWTS | 4.2.1.3 |
| ACPI | There must a DSDT or SSDT. | FWTS | 4.2.1.4 |
| ACPI | MADT must be present and provide GICD and GICC descriptions at the very least. | FWTS |  4.2.1.5 |
| ACPI | For SBSA systems that are >= L1 GTDT must be present and describe the presense of an SBSA Wdog. | FWTS | 4.2.1.6 |
| ACPI | On SBSA systems L1 and higher, DBG2 will be present and define an Generic UART compliant with SBSA Generic UART specification. (Appendix B) | FWTS | 4.2.1.7 |
| ACPI | SPCR should be present. | FWTS | 4.2.1.8 |
| ACPI | SPCR should be version 2, and must have a valid GSIV for the UART SPI. | FWTS | 4.2.1.8 |
| ACPI | Serial port device described in the SPCR must also be present in the DSDT. | FWTS | 4.2.1.8 |
| ACPI | WARN if MCFG is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if IORT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if BERT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if EINJ is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if ERST is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if HEST is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if SPMI is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if SLIT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if SRAT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if PCCT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if SPMI is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if IORT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if SDEI is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if IORT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if PPTT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if PDTT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if PDTT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if NFIT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | WARN if HMAT is not present. | FWTS | Appendix E  4.2.2 |
| ACPI | Platforms define processors as devices (not using Processor keyword) in under the _SB namespace. | FWTS | 4.4.1 |
| ACPI | The _PR namespace is not used to define processors. | FWTS | 4.4.1 |
| ACPI | _SST must be provided. | FWTS | 4.4.1 |
| ACPI | Device method _CCA  must be provided for all devices. | FWTS | 4.4.2 |
| ACPI | Device method _CRS  must be provided for all devices. | FWTS | 4.4.2 |
| ACPI | Device method _HID and/or _ADR must be provided for all devices. A _HID object must be used to describe any device that is enumerated by OSPM. OSPM only enumerates a device when no bus enumerator can detect the ID. For example, devices on an ISA bus are enumerated by OSPM. Use the _ADR object to describe devices that are enumerated by bus enumerators other than OSPM. | FWTS | 4.4.2 |
| ACPI | Device method _STA  must be provided for all devices. | FWTS | 4.4.2 |
| ACPI | Device method _UID  must be provided for all devices and be persistent across boot. | FWTS | 4.2.2 |
| ACPI | GPIO controllers must be provided to implement ACPI event model. _AEI must be present and define GPIO interrupts for ACPI events. _EVT method must be provide to handle these events, and issue notifications. For event numbers lower than 255, _LXX and _EXX methods may be used instead. | FWTS | 4.4.2 |
| ACPI | If ACPI Time and Alarm device is used, it must use the same timebase as UEFI EFI_TIME runtime service. | FWTS | 4.5.2 |
| ACPI | WARN if there are no _CPC xor _PSS methods for all performance domains. | FWTS | Appendix F |
| ACPI | WARN if there are no _LPI methods are not present. | FWTS | Appendix F |
| ACPI | WARN if there are no _IFR is present. | FWTS | Appendix F |
| ACPI | WARN if there are no _SRV is present. | FWTS | Appendix F |
| ACPI | WARN if there are no _PXM is present. | FWTS | Appendix F |
| ACPI | WARN if there are no _SLI is present. | FWTS | Appendix F |
| ACPI | WARN if there are no _CID is present. | FWTS | Appendix F |
| ACPI | WARN if there are no _HMA is present. | FWTS | Appendix F |
| ACPI | Warn for devices that lack an _INI. | FWTS | Appendix F |
| ACPI | Warn is _MLS or _STR are lacking in a device. | FWTS | Appendix F |
| ACPI | Warn is _PRS is missing for a device. | FWTS | Appendix F |
| ACPI | Warn if _SRS is missing for a device. | FWTS | Appendix F |
| ACPI | Interrupt-signaled Events or GPIO-Signaled Events must be provided | FWTS | 4.1 |
| ACPI | _AEI and _EVT methods must be provided for GPIO-signaled event | FWTS | 4.4.3 |
| ACPI | Generic Event Devices with _CRS and _EVT methods for Interrupt-signaled events | FWTS | 4.4.4 |
| ACPI | No WordIo type Address space descriptor for Host to PCI I/O bus. Translation type should be TypeTranslation. | FWTS | 4.4.5 |
| ACPI | SDEI notification type support in APEI HEST table | FWTS | 4.5 |
| SMBIOS | UEFI uses SMBIOS_TABLE_GUID to identify the SMBIOS table. | SCT | 5.1.1 |
| SMBIOS | UEFI uses SMBIOS3_TABLE_GUID to identify the SMBIOS table. | SCT | 5.1.1 |
| SMBIOS | UEFI uses the EfiRuntimeServicesData type for the system memory region containing the SMBIOS tables. | SCT | 5.1.1 |
| SMBIOS | Type00: BIOS Information (REQUIRED) | FWTS | 5.2.1 |
| SMBIOS | Type01: System Information (REQUIRED) | FWTS | 5.2.2 |
| SMBIOS | Type02: Baseboard (or Module) Information (RECOMMENDED) | FWTS | 5.2.3 |
| SMBIOS | Type03: System Enclosure or Chassis (REQUIRED) | FWTS | 5.2.4 |
| SMBIOS | Type07: Cache Information (REQUIRED) | FWTS | 5.2.6 |
| SMBIOS | Type08: Port Connector Information (RECOMMENDED for platforms with physical ports) | FWTS | 5.2.7 |
| SMBIOS | Type09: System Slots (REQUIRED for platforms with expansion slots) | FWTS | 5.2.8 |
| SMBIOS | Type11: OEM Strings (RECOMMENDED) | FWTS | 5.2.9 |
| SMBIOS | Type13: BIOS Language Information (RECOMMENDED) | FWTS | 5.2.10 |
| SMBIOS | Type15: System Event Log (RECOMMENDED) | FWTS | 5.2.11 |
| SMBIOS | Type16: Physical Memory Array (REQUIRED) | FWTS | 5.2.12 |
| SMBIOS | Type17: Memory Device (REQUIRED) | FWTS | 5.2.13 |
| SMBIOS | Type19: Memory Array Mapped Address (REQUIRED) | FWTS | 5.2.14 |
| SMBIOS | Type32: System Boot Information (REQUIRED) | FWTS | 5.2.15 |
| SMBIOS | Type38: IPMI Device Information (REQUIRED for platforms with IPMI BMC Host Interface) | FWTS | 5.2.16 |
| SMBIOS | Type41: Onboard Devices Extended Information (RECOMMENDED) | FWTS | 5.2.17 |
| SMBIOS | Redfish Host interface support (RECOMMENDED) | FWTS | 5.2.18 |
| ACPI | FADT.ARM boot flags.PSCI xor MADT.GICC.PPV must be present. | FWTS | 6 |

