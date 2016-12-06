
# Server Base System Architecture - Architecture Compliance Suite
For more information, download the README.md from (https://github.com/ARM-software/sbsa-acs).

# Server Base Boot Requirements (SBBR)
This Server Base Boot Requirements (SBBR) specification is intended for SBSA[2]-compliant 64-bit ARMv8 servers. It defines the base firmware requirements for out-of-box support of any ARM SBSA-compatible Operating System or hypervisor. The requirements in this specification are expected to be minimal yet complete for booting a multi-core ARMv8 server platform, while leaving plenty of room for OEM or ODM innovations and design details.

For more information, download the [SBBR specification](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0044b/index.html)


## SBBR - Architecture Compliance Suite (ACS)

SBBR **Architecture compliance tests** are self-checking, portable C-based tests with directed stimulus.
The present release implements a UEFI shell application to execute these tests from a UEFI Shell and also implements OS context tests as enhancements to Firmware Test Suite (FWTS).
Both the UEFI Shell based tests and OS Context tests are packaged into Linux UEFI Validation(luvOS) Operating System.
A bootable Linux UEFI Validation(luvOS) Operating System Image is created at the end of build process to ease the process of testing.


## Release Details
 - Code Quality  : Alpha
 - The result of a test should not be taken as a true indication of compliance. There is a possibility of false positives / false negatives.
 - There are gaps in the test coverage. Refer to the [Testcase checklist](docs/testcase-checklist.md) for details.


### Prerequisites

1. Ubuntu 14.04 or 16.04 LTS
2. Install GCC 5.3 or later toolchain for Linux from [here](https://releases.linaro.org/components/toolchain/binaries/)
3. Install the Build pre-requisite packages from [here](https://community.arm.com/docs/DOC-10804)
4. The acs_sync.sh script will check for all required package dependencies.

## Additional Reading
For details on the test coverage, the scenarios implemented in the present release of the ACS and the scenarios planned in the future release, refer to the [Testcase checklist](docs/testcase-checklist.md)

### ACS Build steps

NOTE: Must use BASH shell.

1.  mkdir <container>
2.  cd <container>
3.  git clone https://github.com/ARM-software/arm-enterprise-acs.git
4.  cd arm-enterprise-acs
5.  git checkout master
3.  ./acs_sync.sh -> which will download the luvOS repository and apply the patches.
4.  cd luv
5.  ./build_luvos.sh -> which will build SBBR binaries, SBBR (including UEFI-SCT and FWTS. NOTE: You must be a member of  https://github.com/UEFI/UEFI-SCT as your username and password will be requested.

### Build Output

The luv-live-image.img executable file is generated at:
<container>/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image.img



## Test Suite Execution

The compliance suite execution will vary depending on the Test environment.

1. It will boot to Grub, where it will have two menu entries:
     luvOS (OS context Test cases).
       FWTS will perform default tests.
       Login as "root" and enter the following commands:
         fwts -h (to get run options)
         fwts -a (to run all tests)
     *SBBR/SBSA (Default Choice - UEFI Shell context test cases.)
       SBSA - Enter the following commands:
         FS3:
         cd EFI\BOOT\sbsa
         Sbsa.efi
       SBBR
         The following commands only apply to the first time the image is executed to install the SCT tests)
           FS3:
           cd EFI\BOOT\sbbr
           InstallAARCH64.efi
           2 (to select install destination. Install needs min 100Mb of space)
         The following commands are used after installation of SCT
           FS2:
           cd SCT
           SCT.efi -a -v (to run all tests)
2. User can make a selection and run tests based on his/her choices. See READMEs for SCT, FWTS and SBSA on how to run tests.
   SCT User Guide: download UEFI-Self-Cerification Test(SCT) from (http://www.uefi.org/specs/access)
   FWTS README: (http://kernel.ubuntu.com/git/hwe/fwts.git)
   SBSA README: (https://github.com/ARM-software/sbsa-acs)

3. Logs will be stored into the "luv-results" partition of luv-live-image.img, which can be viewed in any machine after tests are run.


### Juno Reference Platform

Please install the required firmware components for Juno Reference Platform from here:
https://community.arm.com/groups/arm-development-platforms

On a system where a USB port is available and functional, follow the below steps

1. Copy the luv-live-image.img to a USB Flash drive
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


## License

SBBR ACS is distributed under Apache v2.0 License.


## Feedback, Contrubutions and Support

 - Please use the GitHub Issue Tracker associated with this repository for feedback.
 - ARM licensees may contact ARM directly via their partner managers.
 - We welcome code contributions via GitHub Pull requests. Please see "Contributing Code" section of the documentation for details.
