
# SBSA and SBBR - Architecture Compliance Kit (ACK)

## Release details
 - Code Quality : Beta
 - To pick up the release version of the code, checkout the release branch with appropriate tag.
 - To get the latest version of the code with bug fixes and new features, use the master branch.
 - The result of a test should not be taken as a true indication of compliance. There is a possibility of false positives / false negatives.
 - There are gaps in the test coverage. Refer to the [Test case checklist](docs/testcase-checklist.md) for details.

## Introduction
ARM Enterprise ACS, tests compliance against [SBSA](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0029/index.html) and [SBBR](http://infocenter.arm.com/help/topic/com.arm.doc.den0044b/DEN0044B_Server_Base_Boot_Requirements.pdf) specifications.
Architecture Compliance Suites (ACS) are used for ensuring architectural compliance across different implementations/variants 
of the architecture and are delivered in the form of an Architecture Compliance Kit (ACK). 
The ACK is delivered with tests in source form along with a build environment, 
the output of the build being a bootable Linux UEFI Validation (LUV) OS Image 
and documentation on how to run the test suites. This is collectively known as the ARM Architecture Compliance Kit.<br />
<br />
<center><img src="docs/ack-fig1.png"></img></center>
<center>Figure 1 ARM Enterprise ACS product.</center>


Figure 1 illustrates the various components that make up this product.

The tests are available open source. The SBSA components and corresponding abstraction layers are also available open source, with an Apache v2 license allowing for external contribution.

In summary, the ARM Enterprise ACS product contains the following: <ol>
1. Scripts to build, construct and run the test images <br />
2. A bootable LuvOS image capable of running all SBSA and SBBR tests <br />
3. Documentation on how to run the tests <br /> </ol>

The tests included in the product are split between a UEFI application and a Linux driver/application that together prove whether an architectural implementation is compliant with the SBSA
and SBBR specifications.

These tests are described detail below.
## Server Base System Architecture - Architecture Compliance Suite
SBSA specification outlines various system architecture features and software stack functionality that operating systems, hypervisors, and firmware can rely on.
The SBSA test suites check for compliance against the SBSA specification. For more information refer to [SBSA github](https://github.com/ARM-software/sbsa-acs). In summary, The tests are delivered through two bodies of code:
### UEFI Shell SBSA tests
These tests are written on top of Validation and Platform Abstraction Layers (VAL, PAL).

The abstraction layers provide the tests with platform information and runtime environment to enable execution of the tests.
In the present release PAL is written on top of UEFI and ARM Trusted Firmware and for the OS tests, it is written on top of Linux kernel.
Partners may also write their own abstraction layer implementations to allow SBSA tests to be run in other environments, for example as raw workload on an RTL simulation.

### OS SBSA tests
Execution of some SBSA tests requires an OS environment.
This is particularly true for IO tests.
Currently LuvOS is being used but other OS images could be considered in future.

## Server Base Boot Requirements (SBBR)
The Server Base Boot Requirements (SBBR) specification compliments the SBSA specification by defining the base firmware requirements
required for out-of-box support of any SBSA compatible operating system or hypervisor. These requirements are comprehensive enough
to enable booting multi-core 64-bit ARMv8 server platforms while remaining minimal enough to allow for OEM/ODM innovation and
market differentiation.

For more information, refer to the SBBR specification.


### Architecture Compliance Suite (ACS)
**Architecture compliance tests** are self-checking, portable C-based tests with directed stimulus.
This release includes both UEFI Shell and OS context tests that are packaged into a bootable Linux UEFI Validation (LuvOS) image.
The SBBR test suites check for compliance against the SBBR specification. As with the SBSA tests, these tests are also delivered through two bodies of code:
### UEFI Self Certification Tests (SCT)
The SCTs test the UEFI implementation requirements defined by SBBR
Note: The SCTs may eventually merge into EDK2. If this happens SBBR tests in these deliverables will leverage those present in EDK2.

### SBBR based on FWTS
The Firmware Test Suite is a package hosted by Canonical which provides tests for ACPI, SMBIOS and UEFI.
A number of SBBR assertions are tested though FWTS.

## Prerequisites
1. Ubuntu 16.04 LTS with at least 64GB of free disk space.
2. Windows Build Steps will be provided in future releases.

## ACS build steps

Before starting ACS build, make sure of the following: <br />
a. Must use BASH shell.<br />
b. Must be a member of https://github.com/UEFI/UEFI-SCT. if not, please request access by email: admin@uefi.org by providing GitHub account Id.<br />
Note: These build steps only target AArch64. <br />

 #Create a directory that will be your workspace and `cd' into it. <br />
$ mkdir &lt;work_dir&gt; && cd &lt;work_dir&gt; <br />
 #Clone the ARM Enterprise ACS source code <br />
$ git clone https://github.com/ARM-software/arm-enterprise-acs.git <br />
$ cd arm-enterprise-acs <br />
$ git checkout master <br />
 #Download and patch LuvOS source code <br />
$ ./acs_sync.sh <br />
$ cd luv <br />
 #Build LuvOS and test binaries <br />
$ ./build_luvos.sh

Note: The build script provides the option to append kernel command line parameters, if needed. Just press enter to continue with default parameters.

## Build output
The luv-live-image-gpt.img bootable image can be found in:
&lt;work_dir&gt;/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img<br />

This image comprises two FAT file system partitions recognized by UEFI: <br />
- 'luv-results' <br />
  Stores logs and is used to install UEFI-SCT. (Approximate size: 120 MB) <br/>
- 'boot' <br />
  Contains bootable applications and test suites. (Approximate size: 60 MB)

For more information, please see the [Yocto Project](https://www.yoctoproject.org/documentation) and [LuvOS](https://github.com/01org/luv-yocto). <br />


## Juno Reference Platform

Follow the instructions [here](https://community.arm.com/docs/DOC-10804) to install an EDK2 (UEFI) prebuilt configuration on your Juno board.
For additional information, please refer to the FAQs and tutorials [here](https://community.arm.com/groups/arm-development-platforms) or contact [juno-support@arm.com](mailto:juno-support@arm.com).


Next follow the steps below:

1. Burn the LuvOS bootable image to a USB stick: <br />
$ lsblk <br />
$ sudo dd if=&lt;sbsa&gt;/luv-live-image-gpt.img of=/dev/sdX <br />
$ sync <br />
Note: Replace '/dev/sdX' with the handle corresponding to your
  USB stick as identified by the `lsblk' command.
2. Insert the USB stick into one of the Juno's rear USB ports.
3. Power cycle the Juno.

## Fixed Virtual Platform (FVP) environment

The steps for running the ARM Enterprise ACS on an FVP are the
same as those for running on Juno but with a few exceptions:

- Follow the different instructions [here](https://community.arm.com/dev-platforms/b/documents/posts/using-linaros-deliverables-on-an-fvp) to install an EDK2 (UEFI) prebuilt configuration on your FVP
- Modify 'run_model.sh' to add a model command argument that will
  load 'luv-live-image-gpt.img' as a virtual disk image. For example
  if running on the AEMv8-A Base Platform FVP find this section:

cmd="$MODEL \ <br />
-C pctl.startup=0.0.0.0 \ <br />
-C bp.secure_memory=$SECURE_MEMORY \ <br />
$cores \ <br />
-C cache_state_modelled=$CACHE_STATE_MODELLED \ <br />-C bp.pl011_uart0.untimed_fifos=1 \ <br />
-C bp.pl011_uart0.out_file=$UART0_LOG \ <br />
-C bp.pl011_uart1.out_file=$UART1_LOG \ <br />
-C bp.secureflashloader.fname=$BL1 \ <br />
-C bp.flashloader0.fname=$FIP \ <br />
  $image_param \ <br />
  $dtb_param \ <br />
  $initrd_param \ <br />
  -C bp.ve_sysregs.mmbSiteDefault=0 \ <br />
  -C bp.ve_sysregs.exit_on_shutdown=1 \ <br />
  $disk_param \ <br />
  $VARS \ <br />
  $net \ <br />
  $arch_params <br />
  "

  And add `bp.virtioblockdevice.image path=&lt;work_dir&gt;/arm-enterprise-
  acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img' to
  the end:

cmd="$MODEL \ <br />
-C pctl.startup=0.0.0.0 \ <br />
-C bp.secure_memory=$SECURE_MEMORY \ <br />
 $cores \ <br />
-C cache_state_modelled=$CACHE_STATE_MODELLED \ <br />
-C bp.pl011_uart0.untimed_fifos=1 \ <br />
-C bp.pl011_uart0.out_file=$UART0_LOG \ <br />
-C bp.pl011_uart1.out_file=$UART1_LOG \ <br />
-C bp.secureflashloader.fname=$BL1 \ <br />
-C bp.flashloader0.fname=$FIP \ <br />
$image_param \ <br />
$dtb_param \ <br />
$initrd_param \ <br />
-C bp.ve_sysregs.mmbSiteDefault=0 \ <br />
-C bp.ve_sysregs.exit_on_shutdown=1 \ <br />
$disk_param \ <br />
$VARS \ <br />
$net \ <br />
$arch_params \ <br />
bp.virtioblockdevice.image path=&lt;work_dir&gt;/arm-enterprise-acs/luv/
build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img \
"

## Test suite execution
The compliance suite execution will vary depending on the test environment.
The next set of commands are an example of our typical run of the test suites.
Please note that the File System Partition in your platform may vary.

The live image boots to UEFI Shell. The different test applications can be run in following order:

### SBSA
 Enter the following commands, to run the SBSA test on UEFI:

- Shell>FS2:
- FS2:>FS3:\EFI\BOOT\sbsa\sbsa.nsh

If any failures are encountered, refer to [SBSA User Guide](https://github.com/ARM-software/sbsa-acs/raw/master/docs/SBSA_ACS_User_Guide.pdf) for additional debug options.
Power reset the system after completion of this test, and continue with the next step.

Note: Here FS2: is assumed to be the 'luv-results' partition, and FS3: the 'boot' partition
The following commands only apply to the first time the image is executed to install the SCT tests)

### SBBR

- Shell>FS3:
- FS3:>cd EFI\BOOT\sbbr
- FS3:\EFI\BOOT\sbbr>InstallAARCH64.efi

Choose the partition to install SCT on. In a typical run it is FS2:, the 'luv-results' partition.

Enter the following commands are used after installation of SCT:

- Shell>FS2:
- FS2:>cd SCT <br/>
 #To run all tests
- FS2:\SCT>SCT.efi -a -v

User can make a selection and run tests based on available choices. Refer to [SCT User Guide](http://http://www.uefi.org/testtools) on how to run tests.


### luvOS

You can choose to boot luvOS by entering the following commands:

- Shell>FS3:
- FS3:>EFI\BOOT\bootaa64.efi

This command loads the grub menu. You will see one boot option 'luv' to boot luvOS. Press enter to choose it. <br />
That will boot luvOS, run FWTS tests and OS context SBSA tests automatically. <br />


Logs will be stored into the "luv-results" partition, which can be viewed in any machine after tests are run.
   For more information, please refer to [YOCTO documentation](https://www.yoctoproject.org/documentation), or [YOCTO source code](https://github.com/01org/luv-yocto) <br />


## Baselines for Open Source Software in this release:

- [Linux UEFI Validation OS](https://github.com/01org/luv-yocto)
        - SHA: 6a81889e91dd8373b9840350f69332416be0ab0b

- [Firmware Test Suite (FWTS) TAG: V17.03.00](http://kernel.ubuntu.com/git/hwe/fwts.git)

- [Server Base System Architecture (SBSA)](https://github.com/ARM-software/sbsa-acs) TAG: d79cf4415f4a50e7825c27db1a666cfbb4cc01d3

- [UEFI Self Certification Tests (UEFI-SCT)](https://github.com/UEFI/UEFI-SCT) TAG: c78ea66cb114390e8dd8de922bdf4ff3e9770f8c


Note: You must be a member of [UEFI-SCT](https://github.com/UEFI/UEFI-SCT); contact [admin@uefi.org](mailto:admin@uefi.org) and
  provide your GitHub ID to request access.If UEFI-SCT is upstreamed into EDK2 then this will no longer  be required

## Validation

a. Tests run on Juno Reference Platforms

         i. SBSA Tests. (UEFI Shell based tests)
        ii. SBSA Tests. (OS based tests built on top of Linux kernel)
       iii. SBBR Tests. (UEFI Shell based tests built on top of UEFI-SCT Framework)
        iv. SBBR Tests. (OS based tests built on top of FWTS Framework)


b. Known issues

 - Running FWTS EFI Runtime Service monotonic counter tests on Juno
          may cause a crash/hang during the subsequent boot; this is due to
          the destructive nature of the test. To work around this issue,
          enter the following commands into the Juno's boot monitor before
          booting the board again:

         Cmd> flash
         Flash> eraseall
         Flash> quit
         Cmd> reboot

 -  SBSA may hang on "Starting Power and Wakeup semantic tests".

 -  SBBR (UEFI-SCT) Timer tests may hang. It can be recovered by resetting the system,
           in case any of the tests hang.
 -  On Juno r2, running FWTS causes kernel panic at random times during test. Reset the system to restart the test.

c. Planned enhancements
       Upstream SBBR code to open source repositories FWTS and UEFI-SCT.


## License

ARM Enterprise ACS is distributed under Apache v2.0 License.


## Feedback, contributions and support

 - Please use the GitHub Issue Tracker associated with this repository for feedback.
 - ARM licensees may contact ARM directly via their partner managers.
 - We welcome code contributions via GitHub pull requests. Please see "docs/Contributions.txt" for details.
