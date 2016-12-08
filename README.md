
# SBSA and SBBR - Architecture Compliance Kit (ACK).
ARM Enterprise ACS, tests compliance against SBSA and SBBR specifications [SBSA, SBBR].
Architecture Compliance Suites (ACS) are used for ensuring architectural compliance across different implementations/variants 
of the architecture and delivered in the form of an Architecture Compliance Kit (ACK). 
The ACK is delivered with tests in source form along with a build environment, 
the output of the build being a bootable Linux UEFI Validation (LUV) OS Image 
and documentation on how to run the test suites. This is collectively known as the ARM Architecture Compliance Kit.<br />
<br />
|--------------------------------------------------------------|<br />
|                                                              |<br />
|              ------------- LuvOS --------------              |<br />
|              |FWTS (SBBR)|       |SBSA|       |              |<br />
|              ----------------------------------              |<br />
|                                                              |<br />
|              ------------ UEFI Shell ----------              |<br />
|              |SCT (SBBR)|        |SBSA|       |              |<br />
|              ----------------------------------              |<br />
|                                                              |<br />
|--------------------------------------------------------------|<br />
Figure 1 ARM Enterprise ACS product.
<br />
Figure 1 illustrates the various components that make up this product. These are described below<br />
SBSA tests:<br />
The SBSA test suites are check for compliance against the [SBSA] specification. The tests are delivered through two bodies of code:<br />
    UEFI Shell SBSA tests: These tests are written on top of Validation and Platform Adaptation Layers (VAL, PAL).<br />
    The abstraction layers provide the tests with platform information and runtime environment to enable execution of the tests.<br />
    In the case of ARM's deliveries the VAL and PAL shall be written on top of UEFI and ARM Trusted Firmware.<br />
    Partners may also write their own abstraction layer implementations to enable SBSA tests to be executed in other environments, 
    for example as raw workload on an RTL simulation.<br />
<br />
    OS SBSA tests:  It is anticipated that some SBSA tests may require being written in a full OS environment.<br />
    This is particularly true for IO tests. These tests would be written on top of a simple test API and run as tests scripts in an OS image.<br />
    Currently LuvOS is being used but other OS images could be considered in future.<br />

SBBR tests:<br />
The SBBR test suites are check for compliance against the [SBBR] specification. The tests are delivered through two bodies of code:<br />
    SBBR tests contained in UEFI Self Certification Tests (SCT) tests. Many requirements in SBBR are UEFI implementation requirements which are tested by SCT.<br />
    Note that that time of writing there is a possibility that SCT tests may move to EDK2. In this case SBBR tests will leverage EDK2 tests.<br />
<br />
    SBBR based on FWTS: The Firmware Test Suite is a package hosted by Canonical which provides tests for ACPI, SMBIOS and UEFI.<br />
    A number of SBBR assertions are tested though FWTS.<br />

The tests sources are delivered in an open source fashion. SBSA and the abstraction layers are opened sourced, with an appropriate apache v2 license 
that allows external contributions.<br />
SBBR SCT and FWTS tests shall be up streamed into SCT (or EDK2 if things change) and FWTS up streams.<br />
Finally a number of scripts needed to initiate testing, or construct test images are also be included in this open source release.<br />
To enable a unified tests experience,<br />
    1. Scripts to build the tests and create appropriate test images.<br />
    2. Bootable LuvOS image that can run all SBSA and SBBR tests.<br />
    3. Documentation on how to run these tests.<br />

Overall, the ARM Enterprise ACS product contains architectural tests presented as a UEFI application and a Linux Driver/Application to 
prove compliance to the SBSA and SBBR specifications. Architectural implementations are recommended to sign off against the ACS to prove compliance to these specifications. 


# Server Base System Architecture - Architecture Compliance Suite
The Server Base Systems Architecture (SBSA) specification specifies hardware system architecture, based on ARM 64-bit 
architecture, which server system software, such as operating systems, hypervisors and firmware can rely on. It addresses PE 
features and key aspects of system architecture.
For more information, please refer (https://github.com/ARM-software/sbsa-acs).

# Server Base Boot Requirements (SBBR)
This Server Base Boot Requirements (SBBR) specification is intended for SBSA[2]-compliant 64-bit ARMv8 servers. 
It defines the base firmware requirements for out-of-box support of any ARM SBSA-compatible Operating System or hypervisor. 
The requirements in this specification are expected to be minimal yet complete for booting a multi-core ARMv8 server platform, 
while leaving plenty of room for OEM or ODM innovations and design details.

For more information, download the [SBBR specification](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0044b/index.html)


## Architecture Compliance Suite (ACS)
**Architecture compliance tests** are self-checking, portable C-based tests with directed stimulus.
The present release implements a UEFI shell application to execute these tests from a UEFI Shell and also implements OS context tests as enhancements to Firmware Test Suite (FWTS).
Both the UEFI Shell based tests and OS Context tests are packaged into Linux UEFI Validation (LUVOS) Operating System.
A bootable Linux UEFI Validation(luvOS) Operating System Image is created at the end of build process to ease the process of testing.


## Release Details
 - Code Quality : Alpha
 - The result of a test should not be taken as a true indication of compliance. There is a possibility of false positives / false negatives.
 - There are gaps in the test coverage. Refer to the [Testcase checklist](docs/testcase-checklist.md) for details.


### Prerequisites
1. Ubuntu 14.04 or 16.04 LTS
2. Install GCC 5.3 or later toolchain for Linux from [here](https://releases.linaro.org/components/toolchain/binaries/)
3. Install the Build pre-requisite packages from [here](https://community.arm.com/docs/DOC-10804)
4. The acs_sync.sh script will check for all required package dependencies.
5. Windows Build Steps will be provided in future releases.

## Additional Reading
For details on the test coverage, the scenarios implemented in the present release of the ACS and the scenarios planned in the future release, refer to the [Testcase checklist](docs/testcase-checklist.md)

### ACS Build steps
NOTE 1: Must use BASH shell.<br />
Note 2: Must be a member of https://github.com/UEFI/UEFI-SCT. if not, please request access by email: admin@uefi.org by providing GitHub account Id.<br />

1.  mkdir "preferred directory"
2.  cd "preferred directory"
3.  git clone https://github.com/ARM-software/arm-enterprise-acs.git
4.  cd arm-enterprise-acs
5.  git checkout master
6.  ./acs_sync.sh -> which will download the luvOS repository and apply the patches.
7.  cd luv
8.  ./build_luvos.sh -> which will build SBSA/SBBR binaries, SBBR (including UEFI-SCT and FWTS. NOTE: You must be a member of  https://github.com/UEFI/UEFI-SCT as your username and password will be requested.

### Build Output
The luv-live-image.img executable file is generated at:
"preferred directory"/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image.img

For more information, please refer <br />
https://www.yoctoproject.org/documentation <br />
https://github.com/01org/luv-yocto <br />


## Test Suite Execution
The compliance suite execution will vary depending on the Test environment.

1. It will boot to Grub, where it will have two menu entries: <br />
   - luvOS (OS context Test cases).<br />
      - FWTS will perform default tests.<br />
      -  Login as "root" and enter the following commands: <br />
      -  fwts -h (to get run options) <br />
      -  fwts -a (to run all tests) <br />
   - SBBR/SBSA (Default Choice - UEFI Shell context test cases.) <br />

Note: The next set of commands are an example of our typical run of the test suites.<br />
Please note that the File System Partition in your platform may vary.<br />

   - SBSA - Enter the following commands: <br />
      - FS3: <br />
      - cd EFI\BOOT\sbsa <br />
      - Sbsa.efi <br />
<br />
   - SBBR <br />
      - The following commands only apply to the first time the image is executed to install the SCT tests) <br />
        - FS3: <br />
        - cd EFI\BOOT\sbbr <br />
        - InstallAARCH64.efi <br />
        - 2 (to select install destination. Install needs min 100Mb of space) <br />

      - The following commands are used after installation of SCT <br />
        - FS2: <br />
        - cd SCT <br />
        - SCT.efi -a -v (to run all tests) <br />

2. User can make a selection and run tests based on his/her choices. See READMEs for SCT, FWTS and SBSA on how to run tests.<br />
   SCT User Guide: download UEFI-Self-Cerification Test(SCT) from (http://http://www.uefi.org/testtools) <br />
   FWTS README: (http://kernel.ubuntu.com/git/hwe/fwts.git) <br />
   SBSA README: (https://github.com/ARM-software/sbsa-acs) <br />

3. Logs will be stored into the "luv-results" partition, which can be viewed in any machine after tests are run.
   For more information, please refer <br />
   https://www.yoctoproject.org/documentation <br />
   https://github.com/01org/luv-yocto <br />


### Juno Reference Platform

Please install the required firmware components for Juno Reference Platform from here:
https://community.arm.com/groups/arm-development-platforms

On a system where a USB port is available and functional, follow the below steps

1. Copy the luv-live-image.img to a USB Flash drive.
   a. Mount a USB drive in Linux distro being used.
   b. Use dmesg to determine where the USB drive is mounted.
   c. Use dd to transfer file to USB drive. (For example, sudo dd if=luv-live-image.img of=/dev/sdb). Two partitions are created - luv-results and boot.
2. Plug in the USB Flash drive with luv-live-image.img into usb slot on Juno board.
3. Power on board or press Reset button on back.
4. After Juno has booted and the message "Press ESCAPE for boot options Warning: LAN9118 Driver in stopped stat", press ESCAPE to access the Juno Boot Options menu.
5. Select the Boot Manager Menu and see if USB FLASH Drive is the first device that is booted. If not then move it to top of list.
6. Press RETURN/ENTER to continue.

### Emulation environment with secondary storage
Please install the required firmware components

luv-live-image.img can be loaded as virtual disk image. (For example, in ARMv8 FVP Base Models, the image can be loaded with the FVP model argument: 
bp.virtioblockdevice.image_path="<Path to luv-live-image.img>"

## Baselines for Open Source Software in this release:

        (*) Linux Uefi Validation OS (LuvOS) SHA: c60e8de440c832708f70ac953c801150a4963262
            https://github.com/01org/luv-yocto
            (Patched in the release to add SBSA and SBBR support)

        (*) Firmware Test Suite (FWTS) TAG: V16.11.00
            http://kernel.ubuntu.com/git/hwe/fwts.git
            (Patched in the release to add SBBR support)

        (*) Server Base System Architecture (SBSA) TAG: 7e31fd1557522cc8ab23dabc74e402b891a95713
            https://github.com/ARM-software/sbsa-acs

        NOTE: At the time of writing, You must be a member of
        https://github.com/UEFI/UEFI-SCT to access UEFI Self Certification Tests.
        There is a possibility that this will migrate to tianocore edk2
        repository in future. (https://github.com/tianocore/edk2/)
        (*) UEFI Self Certification Tests (UEFI-SCT) TAG: c78ea66cb114390e8dd8de922bdf4ff3e9770f8c
            https://github.com/UEFI/UEFI-SCT
            (Patched in the release to add SBBR support)

## Validation

    a. Tests run
      On Juno Reference Platforms, 
        i. SBSA Tests. (UEFI Shell based Tests)
       ii. SBBR Tests. (UEFI Shell based Tests built on top of UEFI-SCT Framework)
      iii. SBBR Tests. (OS based tests built on top of FWTS Framework)


    b. Known issues
        On Juno, When FWTS EFI Runtime services Tests specifically associated with monotonic
        counter are run, Juno boot crashes the next time, it is booted. This is because
        of destructive nature of the test.
        To get around this, Stop the Autoboot by pressing Enter/Return and flash eraseall
        command needs to be executed from the Command prompt, before booting again.

    c. Planned enhancements
       Upstream SBBR code to open source repositories FWTS and UEFI-SCT.


## License

ARM Enterprise ACS is distributed under Apache v2.0 License.


## Feedback, Contributions and Support

 - Please use the GitHub Issue Tracker associated with this repository for feedback.
 - ARM licensees may contact ARM directly via their partner managers.
 - We welcome code contributions via GitHub Pull requests. Please see "Contributions.txt" file in the docs folder for details.
