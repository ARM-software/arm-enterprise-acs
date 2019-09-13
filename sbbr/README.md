# Server Base Boot Requirements - Architecture Compliance Suite
SBBR specification complements the SBSA specification by defining the base firmware requirements
required for out-of-box support of any SBSA compatible operating system or hypervisor. These requirements are comprehensive enough
to enable booting multi-core 64-bit ARMv8 server platforms while remaining minimal enough to allow for OEM and ODM innovation, and
market differentiation.

For more information, see [SBBR specification](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0044c/index.html).

This release includes both UEFI Shell and OS context tests that are packaged into a bootable LUV OS image.
The SBBR test suites check for compliance against the SBBR specification. Like the SBSA tests, these tests are also delivered through two runtime executable environments:
  - UEFI Self Certification Tests
  - SBBR based on Firmware Test Suite

## UEFI Self Certification Tests
Self-Certification Tests (SCTs) test the UEFI implementation requirements defined by SBBR. The SCT implementation can eventually merge into the EDK2 tree and as a result, SBBR tests in these deliverables leverage those present in EDK2.

**Prerequisite** : Ensure that the system time is correct before starting SCT tests.

### Running SCT
SBBR SCT tests are built as part of the test suite <br />

Running of SBBR SCT tests is now automated. You can choose to skip the automated SCT tests by pressing any key when the UEFI shell prompts.

- Shell>Press any key to stop the EFI SCT running

To run SCT manually, Follow these steps: 

Enter the following commands to install SCT.

- Shell>FS3:
- FS3:>cd EFI\BOOT\sbbr
- FS3:\EFI\BOOT\sbbr>InstallAARCH64.efi

Choose the partition to install SCT on. In a typical run it is FS2:, the 'luv-results' partition.

Enter the following commands after installation of SCT:

- Shell>FS2:
- FS2:>cd SCT <br/>
 #To run all tests
- FS2:\SCT>SCT.efi -a -v

User can select and run tests based on available choices. For information about running the tests, see [SCT User Guide](http://www.uefi.org/testtools).


## SBBR based on Firmware Test Suite
Firmware Test Suite (FWTS) is a package that is hosted by Canonical. FWTS provides tests for ACPI, SMBIOS and UEFI.
Several SBBR assertions are tested though FWTS.

### Running FWTS tests

You can choose to boot LUV OS by entering the command:

- Shell>exit

This command loads the grub menu. Press enter to choose the option 'luv' that boots LUV OS and runs FWTS tests and OS context SBSA tests automatically. <br />

Logs are stored into the "luv-results" partition, which can be viewed on any machine after tests are run.
   For more information, see [YOCTO documentation](https://www.yoctoproject.org/documentation), or [YOCTO source code](https://github.com/01org/luv-yocto) <br />

## Debug Information
SBBR source directories can be found at the following paths:

- SBBR FWTS source at /path/to/arm-enterprise-acs/luv/build/tmp/work/qemuarm64-oe-linux/fwts/V18.02.00+gitAUTOINC+f0afecfc70-r0/git/
- SBBR SCT source at /path/to/arm-enterprise-acs/luv/build/tmp/work/aarch64-oe-linux/sbbr/v1.0+gitAUTOINC+1ff61591f1-r0/git/

To compile and test changes in the above source code, follow these steps:
- cd /path/to/arm-enterprise-acs/luv
- source oe-init-build-env

For FWTS,
- bitbake fwts -f -c compile
- bitbake fwts

For SCT,
- export SCTOPTIONAL=”no”
- export BB_ENV_EXTRAWHITE="BB_ENV_EXTRAWHITE SCTOPTIONAL"
- bitbake sbbr -f -c compile
- bitbake sbbr

After this, run the following command to create an updated luv live image:
- bitbake luv-live-image

## Validation

a. Tests run on SGI-575 Reference Platforms

       i. SBBR Tests. (UEFI Shell based tests built on top of UEFI-SCT Framework)
       ii. SBBR Tests. (OS based tests built on top of FWTS Framework)


b. Known issues
UEFI-SCT Timer tests might hang. They can be recovered by resetting the system.


