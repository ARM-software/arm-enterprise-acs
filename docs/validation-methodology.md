# SBBR ACS Validation Methodology


## About the SBBR
    This Server Base Boot Requirements (SBBR) specification is intended for SBSA[2]-compliant 64-bit ARMv8 servers. It defines the base firmware requirements for out-of-box support of any ARM SBSA-compatible Operating System or hypervisor. The requirements in this specification are expected to be minimal yet complete for booting a multi-core ARMv8 server platform, while leaving plenty of room for OEM or ODM innovations and design details.

This specification is intended to be OS-neutral. It leverages the prevalent industry standard firmware specifications of UEFI and ACPI. Several changes in support of ARMv8 platforms have been submitted to the UEFI working groups of the ARM Bindings Sub Team (ABST) and the ACPI Specification Working Group (ASWG) for review, inclusion, and publication.

## Terms are Abbreviations


This document uses the following terms and abbreviations.

| Term | Meaning                                     |
|------|---------------------------------------------|
| ACPI | Advanced Configuration and Power Interface  |
| GIC  | Generic Interrupt Controller                |
| SPI  | Shared Peripheral Interrupt                 |
| PPI  | Private Peripheral Interrupt                |
| LPI  | Locality Specific Peripheral Interrupt      |
| XSDT | Extended System Description Table           |
| SMC  | Secure Monitor Call                         |
| PE   | Processing Element                          |
| PSCI | Power State Coordination Interface          |
| PCIe | Peripheral Component Interconnect - Express |
| SBSA | Server Base Systems Architecture            |
| UART | Universal Asynchronous Receiver/Transmitter |
| UEFI | Unified Extensible Firmware Interface       |


## Additional reading

  This document refers to the following external documents.
  1. http://www.uefi.org/specifications
    - ACPI Specification Version 6.1
    - UEFI Specification Version 2.6
    - UEFI Platform Initialization Specification Version 1.4 (Volume 2)
  2.  http://www.uefi.org/acpi
    - WDRT
    - MCFG
    - IORT
    - DBG2

See Infocenter: http://infocenter.arm.com for access to ARM documentation

**ARM Publications**

  This document refers to the following documents:
    1. Server Base System Architecture (ARM-DEN-0029 Version 3.0)
    2. Server Base Boot Requirements (ARM-DEN-0044B)
    3. ARM® Architecture Reference Manual ARMv8, for ARMv8-A architecture profile (ARM DDI 0487).


## Compliance Test Suite

SBBR compliance tests are self-checking, portable C-based tests which are based on the Server Base Boot Requirements specification. The test suite is divided into two components:

1. UEFI-SCT
2. Firmware Test Suite (FWTS)
3. SBSA



