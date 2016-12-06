# SBSA ACS Testcase checklist

Component		SBBR version? 	Test assertion			App/SCT/FWTS	SBBR Section		SCT		Source file	Test Description
UEFI - BOOT SERVICES		1-	Memory map will be identify	App		3.4.1			
UEFI - BOOT SERVICES		1-	All devices and memories that are described in the UEFI memory map must be 64Kb aligned	App	3.4.1			
UEFI - BOOT SERVICES		1-	Default RAM attribute must be EFI_MEMORY_WB	App	3.4.1			
UEFI - BOOT SERVICES		1-	EFI_ACPI_20_TABLE_GUID must be present in system configuration table	App	3.4.4			
UEFI - BOOT SERVICES		1	SMBIOS3_TABLE_GUID must be present in system configuration table	App	3.4.4			
UEFI - BOOT SERVICES		0.9	SMBIOS_TABLE_GUID must be present in system configuration table	App	3.4.4			
UEFI - BOOT SERVICES		1-	EFI_CREATE_EVENT tests		SCT		Appendix A	5.1.1.1.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Functional test for boot service CreateEvent()
UEFI - BOOT SERVICES		1-	EFI_CLOSE_EVENT tests		SCT		Appendix A	5.1.1.2.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Functional test for boot service CloseEvent()
UEFI - BOOT SERVICES		1-	EFI_SIGNAL_EVENT tests		SCT		Appendix A	5.1.1.3.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Functional test for boot service SignalEvent()
UEFI - BOOT SERVICES		1-	EFI_WAIT_FOR_EVENT tests	SCT		Appendix A	5.1.1.4.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Conformance test for boot service WaitForEvent()
UEFI - BOOT SERVICES		1-	EFI_SET_TIMER tests		SCT		Appendix A	5.1.1.6.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Conformance test for boot service SetTimer()
UEFI - BOOT SERVICES		1-	EFI_RAISE_TPL tests		SCT		Appendix A	5.1.1.7.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Functional test for boot service RaiseTPL()
UEFI - BOOT SERVICES		1-	EFI_RESTORE_TPL tests		SCT		Appendix A	5.1.1.8.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Functional test for boot service RaiseTPL()
UEFI - BOOT SERVICES		1-	EFI_CREATE_EVENT_EX tests	SCT		Appendix A	5.1.1.9.*	SctPkg/TestCase/UEFI/EFI/BootServices/EventTimerTaskPriorityServices/BlackBoxTest/EventTimerTaskPriorityServicesBBTest_uefi.inf		Functional test for boot service CreateEventEx()
UEFI - BOOT SERVICES		1-	EFI_ALLOCATE_PAGES tests	SCT		Appendix A	5.1.2.1.*	SctPkg/TestCase/UEFI/EFI/BootServices/MemoryAllocationServices/BlackBoxTest/MemoryAllocationServicesBBTest.inf				"Consistency Test for AllocatePages, Interface Test for AllocatePages"
UEFI - BOOT SERVICES		1-	EFI_FREE_PAGES tests		SCT		Appendix A	5.1.2.2.*	SctPkg/TestCase/UEFI/EFI/BootServices/MemoryAllocationServices/BlackBoxTest/MemoryAllocationServicesBBTest.inf				"Consistency Test for FreePages, Interface Test for FreePages"
UEFI - BOOT SERVICES		1-	EFI_GET_MEMORY_MAP tests	SCT		Appendix A	5.1.2.3.*	SctPkg/TestCase/UEFI/EFI/BootServices/MemoryAllocationServices/BlackBoxTest/MemoryAllocationServicesBBTest.inf				"Consistency Test for GetMemoryMap, Interface Test for GetMemoryMap"
UEFI - BOOT SERVICES		1-	EFI_ALLOCATE_POOL tests		SCT		Appendix A	5.1.2.4.*	SctPkg/TestCase/UEFI/EFI/BootServices/MemoryAllocationServices/BlackBoxTest/MemoryAllocationServicesBBTest.inf				"Consistency Test for AllocatePool, Interface Test for AllocatePool"
UEFI - BOOT SERVICES		1-	EFI_FREE_POOL tests		SCT		Appendix A	5.1.2.5*	SctPkg/TestCase/UEFI/EFI/BootServices/MemoryAllocationServices/BlackBoxTest/MemoryAllocationServicesBBTest.inf				"Consistency Test for FreePool, Interface Test for FreePages"
UEFI - BOOT SERVICES		1-	EFI_INSTALL_PROTOCOL_INTERFACE tests	SCT	Appendix A	5.1.3.1.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for InstallProtocolInterface, Interface Function Test for InstallProtocolInterface"
UEFI - BOOT SERVICES		1-	EFI_UNINSTALL_PROTOCL_INTERFACE tests	SCT	Appendix A	5.1.3.2.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for UninstallProtocolInterface, Interface Function Test for UninstallProtocolInterface"
UEFI - BOOT SERVICES		1-	EFI_REINSTALL_PROTOCOL_INTERFACE tests	SCT	Appendix A	5.1.3.3.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for ReinstallProtocolInterface, Interface Function Test for ReinstallProtocolInterface"
UEFI - BOOT SERVICES		1-	EFI_REGISTER_PROTOCOL_NOTIFY tests	SCT	Appendix A	5.1.3.4.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for RegisterProtocolNotify, Interface Function Test for RegisterProtocolNotify"
UEFI - BOOT SERVICES		1-	EFI_LOCATE_HANDLE tests		SCT		Appendix A	5.1.3.5.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					Conformance Test for LocateHandle
UEFI - BOOT SERVICES		1-	EFI_HANDLE_PROTOCOL tests	SCT		Appendix A	5.1.3.6.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for HandleProtocol, Interface Function Test for HandleProtocol"
UEFI - BOOT SERVICES		1-	EFI_LOCATE_DEVICE_PATH tests	SCT		Appendix A	5.1.3.7.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for LocateDevicePath, Interface Function Test for LocateDevicePath"
UEFI - BOOT SERVICES		1-	EFI_OPEN_PROTOCOL tests		SCT		Appendix A	5.1.3.8.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for OpenProtocol, Interface Function Test within test case only for OpenProtocol, Interface Function Test with non-EFI driver model driver for OpenProtocol, Interface Function Test with test drivers following EFI driver model for OpenProtocol"
UEFI - BOOT SERVICES		1-	EFI_CLOSE_PROTOCOL tests	SCT		Appendix A	5.1.3.9.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for CloseProtocol, Interface Function Test for CloseProtocol"
UEFI - BOOT SERVICES		1-	EFI_OPEN_PROTOCOL_INFORMATION tests	SCT	Appendix A	5.1.3.10.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for OpenProtocolInformation, Interface Function Test for OpenProtocolInformation"
UEFI - BOOT SERVICES		1-	EFI_CONNECT_CONTROLLER tests	SCT		Appendix A	5.1.3.11.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for ConnectController, Interface Function Test for ConnectController"
UEFI - BOOT SERVICES		1-	EFI_DISCONNECT_CONTROLLER tests	SCT		Appendix A	5.1.3.12.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for DisconnectController, Interface Function Test for DisconnectController"
UEFI - BOOT SERVICES		1-	EFI_PROTOCOLS_PER_HANDLE tests	SCT		Appendix A	5.1.3.13.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for ProtocolsPerHandle, Interface Function Test for ProtocolsPerHandle"
UEFI - BOOT SERVICES		1-	EFI_LOCATE_HANDLE_BUFFER tests	SCT		Appendix A	5.1.3.14.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for LocateHandleBuffer, Interface Function Test for LocateHandleBuffer"
UEFI - BOOT SERVICES		1-	EFI_LOCATE_PROTOCOL tests	SCT		Appendix A	5.1.3.15.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf					"Conformance Test for LocateProtocol, Interface Function Test for LocateProtocol"
UEFI - BOOT SERVICES		1-	EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES tests	SCT	Appendix A	5.1.3.16.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf				"Conformance Test for InstallMultipleProtocolInterfaces, Interface Function Test for InstallMultipleProtocolInterfaces"
UEFI - BOOT SERVICES		1-	EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES tests	SCT	Appendix A	5.1.3.17.*	SctPkg/TestCase/UEFI/EFI/BootServices/ProtocolHandlerServices/BlackBoxTest/ProtocolHandlerBBTest.inf			"Conformance Test for UninstallMultipleProtocolInterfaces, Interface Function Test for UninstallMultipleProtocolInterfaces"
UEFI - BOOT SERVICES		1-	EFI_IMAGE_LOAD tests		SCT		Appendix A	5.1.4.1.*		
UEFI - BOOT SERVICES		1-	EFI_IMAGE_START tests		SCT		Appendix A	5.1.4.2.*		
UEFI - BOOT SERVICES		1-	EFI_IMAGE_UNLOAD tests		SCT		Appendix A	5.1.4.3.*		
UEFI - BOOT SERVICES		1-	EFI_EXIT tests			SCT		Appendix A	5.1.4.5.*		
UEFI - BOOT SERVICES		1-	EFI_EXIT_BOOT_SERVICES tests	SCT		Appendix A	5.1.4.6.*		
UEFI - BOOT SERVICES		1-	EFI_SET_WATCHDOG_TIMER tests	SCT		Appendix A	5.1.5.1.*		
UEFI - BOOT SERVICES		1-	EFI_STALL tests			SCT		Appendix A	5.1.5.2.*		
UEFI - BOOT SERVICES		1-	EFI_COPY_MEM tests		SCT		Appendix A	5.1.5.3.*		
UEFI - BOOT SERVICES		1-	EFI_SET_MEM tests		SCT		Appendix A	5.1.5.4.*		
UEFI - BOOT SERVICES		1-	EFI_GET_NEXT_MONOTONIC_COUNT tests	SCT	Appendix A	5.1.5.5.*		
UEFI - BOOT SERVICES		1-	EFI_INSTALL_CONFIGURATION_TABLE tests	SCT	Appendix A	5.1.5.6.*		
UEFI - BOOT SERVICES		1-	EFI_CALCULATE_CRC32 tests	SCT		Appendix A	5.1.5.7.*		
UEFI - RUNTIME SERVICES		1-	"all mapped 4K memory pages must have identical page attributes, within the same physical 64K page"	App	3.5.2			
UEFI - RUNTIME SERVICES		1-	EFI_GET_TIME / GetTime() will be provided	App	3.5.3			
UEFI - RUNTIME SERVICES		1-	EFI_SET_TIME / SetTime() will be provided	App	3.5.3			
UEFI - RUNTIME SERVICES		1-	ResetSystem() must be provided	App		3.5.4			
UEFI - RUNTIME SERVICES		1-	Persitent variable services must be provided	SCT	3.5.5	 5.2.1.3.23-26		
UEFI - RUNTIME SERVICES		1-	EFI_GET_VARIABLE tests		SCT		Appendix B	5.2.1.1.*		
UEFI - RUNTIME SERVICES		1-	EFI_GET_NEXT_VARIABLE_NAME tests	SCT	Appendix B	5.2.1.2.*		
UEFI - RUNTIME SERVICES		1-	EFI_SET_VARIABLE tests		SCT		Appendix B	5.2.1.3.*		
UEFI - RUNTIME SERVICES		1-	EFI_QUERY_VARIABLE_INFO tests	SCT		Appendix B	5.2.1.4.*		
UEFI - RUNTIME SERVICES		1-	EFI_GET_TIME tests		SCT		Appendix B	5.2.2.1.*		
UEFI - RUNTIME SERVICES		1-	EFI_SET_TIME tests		SCT		Appendix B	5.2.2.2.*		
UEFI - RUNTIME SERVICES		1-	EFI_GET_WAKEUP_TIME tests	SCT		Appendix B	5.2.2.3.*		
UEFI - RUNTIME SERVICES		1-	EFI_SET_WAKEUP_TIME tests	SCT		Appendix B	5.2.2.4.*		
UEFI - RUNTIME SERVICES		1-	EFI_RESET_SYSTEM		SCT		Appendix B	5.2.4.1.*		
UEFI - RUNTIME SERVICES		1-	EFI_UPDATE_CAPSULE tests	SCT		Appendix B	5.2.4.2.*		
UEFI - RUNTIME SERVICES		1-	EFI_QUERY_CAPSULE_CAPABILITIES tests	SCT	Appendix B	5.2.4.3.*		
UEFI - RUNTIME SERVICES		1-	EFI_SET_VIRTUAL_ADDRESS_MAP tests	OS	Appendix B			
UEFI - RUNTIME SERVICES		1-	EFI_CONVERT_POINTER tests	OS		Appendix B			
UEFI - PROTOCOLS		1-	EFI_LOADED_IMAGE_PROTOCOL	SCT		Appendix C	5.3.1.1.*		
UEFI - PROTOCOLS		1-	EFI_DEVICE_PATH_PROTOCOL 	SCT		Appendix C	5.4.1-2.*.*		
UEFI - PROTOCOLS		1-	EFI_DECOMPRESS_PROTOCOL 	SCT		Appendix C	5.13.1.1-2.*		
UEFI - PROTOCOLS		1-	EFI_DEVICE_PATH_UTILITIES_PROTOCOL	SCT	Appendix C	5.4.3-4.*.*		
UEFI - PROTOCOLS		1-	EFI_LOAD_FILE_PROTOCOL		App		Appendix C			
UEFI - PROTOCOLS		1-	EFI_LOAD_FILE2_PROTOCOL		App		Appendix C			
UEFI - PROTOCOLS		1-	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL	SCT		Appendix C	5.7.2.1.*		
UEFI - PROTOCOLS		1-	EFI_FILE_PROTOCOL		SCT		Appendix C	5.7.3.1-15.*		
UEFI - PROTOCOLS		1-	EFI_SIMPLE_TEXT_INPUT_PROTOCOL	SCT		Appendix C	5.6.1.1.*		
UEFI - PROTOCOLS		1-	EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL 	SCT	Appendix C	5.6.7.1-5.*		
UEFI - PROTOCOLS		1-	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	SCT		Appendix C	5.6.2.1.*		
UEFI - PROTOCOLS		1-	EFI_HII_DATABASE_PROTOCOL 	SCT		Appendix C	5.18.4.1-11.*		
UEFI - PROTOCOLS		1-	EFI_HII_STRING_PROTOCOL		SCT		Appendix C	5.18.2.1-5.*		
UEFI - PROTOCOLS		1-	EFI_HII_CONFIG_ROUTING_PROTOCOL SCT		Appendix C	5.18.5.1-6.*		
UEFI - PROTOCOLS		1-	EFI_HII_CONFIG_ACCESS_PROTOCOL	SCT		Appendix C	5.18.6.1-2.*		
UEFI - PROTOCOLS		1-	EFI_SIMPLE_NETWORK_PROTOCOL 	SCT		Appendix D	5.11.1.1-13.*		
UEFI - PROTOCOLS		1-	EFI_MANAGED_NETWORK_PROTOCOL 	SCT		Appendix D	5.23.1.1-10.*		
UEFI - PROTOCOLS		1-	EFI_MANAGED_NETWORK_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.23.1.1-10.*		
UEFI - PROTOCOLS		1-	EFI_PXE_BASE_CODE_PROTOCOL 	SCT		Appendix D	5.11.2.1-12.*		
UEFI - PROTOCOLS		1-	EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL 	SCT	Appendix D	5.11.2.1-12.*		
UEFI - PROTOCOLS		1-	EFI_MTFTP4_PROTOCOL 		SCT		Appendix D	5.26.2.1-10.*		
UEFI - PROTOCOLS		1-	EFI_MTFTP6_PROTOCOL		SCT		Appendix D	5.26.4.1-10.*		
UEFI - PROTOCOLS		1-	EFI_ARP_PROTOCOL 		SCT		Appendix D	5.24.1.1-10.*		
UEFI - PROTOCOLS		1-	EFI_ARP_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.24.1.1-10.*		
UEFI - PROTOCOLS		1-	EFI_DHCP4_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.24.2.1-11.*		
UEFI - PROTOCOLS		1-	EFI_DHCP4_PROTOCOL 		SCT		Appendix D	5.24.2.1-11.*		
UEFI - PROTOCOLS		1-	EFI_TCP4_PROTOCOL 		SCT		Appendix D	5.25.1.1-25.*		
UEFI - PROTOCOLS		1-	EFI_TCP4_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.25.1.1-25.*		
UEFI - PROTOCOLS		1-	EFI_IP4_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.25.2.1-10.*		
UEFI - PROTOCOLS		1-	EFI_IP4_CONFIG_PROTOCOL 	SCT		Appendix D	5.25.2.1-10.*		
UEFI - PROTOCOLS		1-	EFI_UDP4_PROTOCOL		SCT		Appendix D	5.26.1.1-10.*		
UEFI - PROTOCOLS		1-	EFI_UDP4_SERVICE_BINDING_PROTOCOL	SCT	Appendix D	5.26.1.1-10.*		
UEFI - PROTOCOLS		1-	EFI_DHCP6_PROTOCOL 		SCT		Appendix D	5.24.3.1-11.*		
UEFI - PROTOCOLS		1-	EFI_DHCP6_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.24.3.1-11.*		
UEFI - PROTOCOLS		1-	EFI_TCP6_PROTOCOL 		SCT		Appendix D	5.25.4.1-8.*		
UEFI - PROTOCOLS		1-	EFI_IP6_SERVICE_BINDING_PROTOCOL 	SCT	Appendix D	5.25.5.1-11.*		
UEFI - PROTOCOLS		1-	EFI_IP6_CONFIG_PROTOCOL 	SCT		Appendix D	5.25.5.1-11.*		
UEFI - PROTOCOLS		1-	EFI_UDP6_PROTOCOL 		SCT		Appendix D	5.26.3.1-8.*		
UEFI - PROTOCOLS		1-	EFI_UDP6_SERVICE_BINDING_PROTOCOL	SCT	Appendix D	5.26.3.1-8.*		
UEFI - PROTOCOLS		1-	EFI_VLAN_CONFIG_PROTOCOL	SCT		Appendix D	5.27.1.1-3.*		
UEFI - PROTOCOLS		1-	EFI_ISCSI_INITIATOR_NAME_PROTOCOL	SCT	Appendix D	5.17.1-2.1.*		
UEFI - PROTOCOLS		1-	EFI_EBC_PROTOCOL		SCT		Appendix D	5.15.1.1-4.*		
ACPI				0.9	ACPI must be compliant with version 5.1 or later	FWTS/OS	4.1			
ACPI				1	ACPI must be compliant with version 6.0 or later	FWTS/OS	4.1		fwts/src/acpi/fadt/fadt.c	FADT revision test
ACPI				1-	Implement the HW-Reduced ACPI model.	FWTS/OS	4.1				fwts/src/acpi/fadt/fadt.c	ACPI FADT Description Table tests.
ACPI				1-	Warn if legacy HW interfaces are supported as it should be HW reduced platform	FWTS/OS	4.1/4.2.1.3		fwts/src/acpi/fadt/fadt.c	ACPI FADT Description Table tests.
ACPI				1-	Provide GPIO signalled events for runtime notifications to OSPM	FWTS/OS	4.1			
ACPI				1-	"RSDP must be implemented where RsdtAddress field must be null (zero) and the XsdtAddresss MUST be a valid, non-null, 64-bit value"	FWTS/OS	4.2.1.1		fwts/src/acpi/rsdp/rsdp.c					RSDP Root System Description Pointer test
ACPI				1-	"XSDT must be present, pointed at by XsdrAddress, and contain valid pointers to all other ACPI tables mandated by this spec"	FWTS/OS	4.2.1.2		fwts/src/acpi/xsdt/xsdt.c	XSDT Extended System Description Table test
ACPI				1-	It is recommended that server profile is selected	FWTS/OS	4.2.1.3		fwts/src/acpi/fadt/fadt.c	ACPI FADT Description Table tests
ACPI				1-	There must a DSDT or SSDT	FWTS/OS	4.2.1.4		fwts/src/acpi/acpitables/acpitables.c	Test DSDT and SSDT tables are implemented
ACPI				1-	MADT must be present and provide GICD and GICC descriptions at the very least.	FWTS/OS	4.2.1.5		fwts/src/acpi/madt/madt.c	MADT subtable tests
ACPI				1-	For SBSA systems that are >= L1 						FWTS/OS	4.2.1.6		fwts/src/acpi/gtdt/gtdt.c	GTDT Generic Timer Description Table test
ACPI				1-	For SBSA systems that are >= L1 GTDT must be present and describe the presense of an SBSA Wdog	FWTS/OS	4.2.1.6		fwts/src/acpi/gtdt/gtdt.c	GTDT Generic Timer Description Table test
ACPI				1-	"On SBSA systems L1 and higher, DBG2 will be present and define an Generic UART compliant with SBSA Generic UART specification (Appendix B) "	FWTS/OS	4.2.1.7		fwts/src/acpi/dbg2/dbg2.c	DBG2 (Debug Port Table 2) test
ACPI				1-	SPCR should be present. 	FWTS/OS	4.2.1.8		fwts/src/acpi/spcr/spcr.c	SPCR Serial Port Console Redirection Table test
ACPI				1	"SPCR should be version 2, and must have a valid GSIV for the UART SPI"	FWTS/OS	4.2.1.8		fwts/src/acpi/spcr/spcr.c	Revision 2 is assumed for testing.
ACPI				1	Serial port device described in the SPCR must also be present in the DSDT	FWTS/OS	4.2.1.8		fwts/src/acpi/spcr/spcr.c	DBG2 (Debug Port Table 2) test
ACPI				1-	WARN if MCFG is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if IORT is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if BERT is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if EINJ is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if ERST is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if HEST is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if SPMI is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if SLIT is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if SRAT is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if MPST is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	WARN if PCCT is not present	FWTS/OS	4.2.2/Appendix E		/src/acpi/acpitables/acpitables.c	Check for recommended ACPI tables
ACPI				1-	Platforms define procssors as devices (not using Processor keyword) in under the _SB namespace.	FWTS/OS	4.4.1 		/src/acpi/acpitables/acpitables.c	Test that processors only exist in the _SB namespace
ACPI				1-	The _PR namespace is not used to define processors	FWTS/OS	4.4.1 		/src/acpi/method/method.c	
ACPI				1-	_SST must be provided	FWTS/OS	4.4.1		/src/acpi/method/method.c	SBBR Test _SST (System Status)
ACPI				1-	Device method _CCA  must be provided for all devices	FWTS/OS if it needs to be evaluated 	4.4.2		/src/acpi/method/method.c	Test _CCA (Cache Coherency Attribute)
ACPI				1-	Device method _CRS  must be provided for all devices	FWTS/OS if it needs to be evaluated 	4.4.2		/src/acpi/method/method.c	Test _CRS (Current Resource Settings)
ACPI				1-	"Device method _HID and/or _ADR must be provided for all devices. A _HID object must be used to describe any device that is enumerated by OSPM. OSPM only enumerates a device when no bus enumerator can detect the ID. For example, devices on an ISA bus are enumerated by OSPM. Use the _ADR object to describe devices that are enumerated by bus enumerators other than OSPM."	FWTS/OS if it needs to be evaluated 	4.4.2		/src/acpi/method/method.c	SBBR Test _ADR (Return Unique ID for Device) SBBR Test _HID (Hardware ID)
ACPI				1-	Device method _STA  must be provided for all devices	FWTS/OS if it needs to be evaluated 	4.4.2		/src/acpi/method/method.c	SBBR Test _STA (Status)
ACPI				1-	Device method _UID  must be provided for all devices and be persistent across boot	FWTS/OS if it needs to be evaluated 	4.4.2		/src/acpi/method/method.c	SBBR Test _UID (Unique ID)
ACPI				1-	"GPIO controllers must be provided to implement ACPI event model. _AEI must be present and define GPIO interrupts for ACPI events. _EVT method must be provide to handle these events, and issue notifications. For event numbers lower than 255, _LXX and _EXX methods may be used instead"	FWTS/OS if it needs to be evaluated 	4.4.2		/src/acpi/method/method.c	SBBR Test _AEI (Event Information)
ACPI				1-	"If ACPI Time and Alarm device is used, it must use the same timebase as UEFI EFI_TIME runtime service"	OS	4.5.2		/src/acpi/method/method.c	"Test _GCP (Get Capabilities), Test _GRT (Get Real Time), Test _GWS (Get Wake Status), Test _CWS (Clear Wake Status), Test _SRT (Set Real Time), Test _STP (Set Expired Timer Wake Policy), Test _STV (Set Timer Value), Test _TIP (Expired Timer Wake Policy), Test _TIV (Timer Values)"
ACPI				1-	WARN if there are no _CPC xor _PSS methods for all performance domains 	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	"Test _CPC (Continuous Performance Control), Test _PSS (Performance Supported States)"
ACPI				1	WARN if there are no _LPI methods are not present	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	Test _LPI (Low Power Idle States)
ACPI				1-	WARN if there are no _IFR is present	FWTS/OS if it needs to be evaluated 	Appendix F
ACPI				1-	WARN if there are no _SRV is present	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	Test _SRV (IPMI Interface Revision)
ACPI				1	WARN if there are no _PXM is present	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	Test _PXM (Proximity)
ACPI				1	WARN if there are no _SLI is present	FWTS/OS if it needs to be evaluated 	Appendix F
ACPI				1-	WARN if there are no _CID is present	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	Test _CID (Compatible ID)
ACPI				1-	Warn for devices that lack an _INI	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	Test _INI (Initialize)
ACPI				1-	Warn is _MLS or _STR are lacking in a device	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	"_MLS (Multiple Language String), Test _STR (String)"
ACPI				1-	Warn is _PRS is missing for a device	FWTS/OS if it needs to be evaluated 	Appendix F		/src/acpi/method/method.c	Test _PRS (Possible Resource Settings
ACPI				1-	Warn if _SRS is missing for a device	FWTS/OS if it needs to be evaluated 	Appendix F
SMBIOS				0.9	UEFI uses SMBIOS_TABLE_GUID to identify the SMBIOS table	App	5.1.1
SMBIOS				1	UEFI uses SMBIOS3_TABLE_GUID to identify the SMBIOS table	App	5.1.1
SMBIOS				1-	UEFI uses the EfiRuntimeServicesData type for the system memory region containing the SMBIOS tables. 	App	5.1.1
SMBIOS				1-	Type00: BIOS Information (REQUIRED)				App	5.2.1		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type01: System Information (REQUIRED)				App	5.2.2		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type02: Baseboard (or Module) Information (RECOMMENDED)		App	5.2.3		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type03: System Enclosure or Chassis (REQUIRED)			App	5.2.4		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type07: Cache Information (REQUIRED)				App	5.2.6		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type08: Port Connector Information (RECOMMENDED for platforms with physical ports) App	fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type09: System Slots (REQUIRED for platforms with expansion slots) App			fwts/src/dmi/dmicheck/dmicheck.c 	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type 11: OEM Strings (RECOMMENDED)				App	5.2.9		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type13: BIOS Language Information (RECOMMENDED)			App	5.2.10		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type15: System Event Log (RECOMMENDED)				App	5.2.11		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type16: Physical Memory Array (REQUIRED)			App	5.2.12		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type17: Memory Device (REQUIRED)				App	5.2.13		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type19: Memory Array Mapped Address (REQUIRED)			App	5.2.14		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type32: System Boot Information (REQUIRED)			App	5.2.15		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type38: IPMI Device Information (REQUIRED for platforms with IPMI BMC Host Interface)App 5.2.16 fwts/src/dmi/dmicheck/dmicheck.c Test ARM SBBR SMBIOS structure requirements
SMBIOS				1-	Type41: Onboard Devices Extended Information (RECOMMENDED)	App	5.2.17		fwts/src/dmi/dmicheck/dmicheck.c	Test ARM SBBR SMBIOS structure requirements
Secondary Core Boot / ACPI	1-	FADT.ARM boot flags.PSCI xor MADT.GICC.PPV must be present. 	App	6			

