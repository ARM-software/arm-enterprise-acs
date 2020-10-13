# Memory Partitioning and Monitoring - Architecture Compliance Suite


##Memory Partitioning and Monitoring (MPAM)

**Arm v8.4 MPAM Extension Architecture v1.0** covers optional additions to the Arm architecture to support memory system partitioning. The specification also includes additions to system performance monitoring
for measuring the resource usage by software. Together these facilities permit software to observe memory system usage and allocate resources to software by running the software in a memory system partition.

##Architecture Compliance Suite
**Architecture Compliance Suite** (ACS) is a collection of self-checking, portable C-based tests. This suite includes a set of examples of the invariant behaviors that are provided by the MPAM specification,
so that implementations can verify if these behaviors have been interpreted correctly. The compliance suite is executed from UEFI shell by executing the MPAM UEFI shell application.

## Release details
  - Release version :  v0.5
  - Code quality :  Alpha <br />
    The suite is in development. This indicates that the suite contains tests which have not been validated on any platform. Please use this opportunity to suggest enhancements and point out errors.
  - Tests were written for MPAM v1.0 specification.
  - The compliance suite is not a substitute for design verification.
  - To review the MPAM ACS logs, Arm licensees can contact Arm directly through their partner managers.
  - To know about the gaps in the test coverage, see [Scenario Document](docs/Arm_MPAM_ACS_Scenario_Document.pdf).

## GitHub branch
  - To get the latest version of the code with bug fixes, use the master branch.

## Target platforms
Any AARCH64 Enterprise Platform that boots UEFI

## ACS build steps - UEFI shell application

### Prerequisites
To build ACS, ensure that the following requirements are met.

  - git clone [EDK2 tree](https://github.com/tianocore/edk2).
  - Install GCC 7.1 or later toolchain for Linux from [here](https://releases.linaro.org/components/toolchain/binaries/)
  - Install the build prerequisite packages to build EDK2. The details of the packages are beyond the scope of this document.

Perform following steps before starting the ACS build:

1.  cd local_edk2_path
2.  git clone https://github.com/ARM-software/arm-enterprise-acs.git AppPkg/Applications/mpam
4.  Add following two libraries to [LibraryClasses.common] section in path/to/edk2/ShellPkg/ShellPkg.dsc
    - MpamValLib|AppPkg/Applications/mpam/mpam/val/MpamValLib.inf
    - MpamPalLib|AppPkg/Applications/mpam/mpam/platform/pal_uefi_acpi_override/MpamPalLib.inf
5.  Add AppPkg/Applications/mpam/mpam/uefi_app/MpamAcs.inf in [Components] section in path/to/edk2/ShellPkg/ShellPkg.dsc

### Linux build environment
Perform the following steps to build ACS in Linux environment.

1.  export GCC49_AARCH64_PREFIX=GCC 7.1 or later toolchain path pointing to /bin/aarch64-linux-gnu-
2.  source edksetup.sh
3.  make -C BaseTools/Source/C
4.  source AppPkg/Applications/mpam/mpam/scripts/acsbuild.sh

### Build output

The output EFI executable is generated at local_edk2_path/Build/Shell/DEBUG_GCC49/AARCH64/Mpam.efi

## Test suite execution
The execution of the compliance suite assumes that the suite is invoked through the ACS UEFI shell application.

### Emulation environment with secondary storage
To run the tests on an emulation environment with secondary storage, perform the following steps:

1.  Create an image file to store the shell application file 'Mpam.efi'. For example:
    - $ mkfs.vfat -C -n HD0 hda.img 2097152
    - $ sudo mkdir /mnt/mpam
    - $ sudo mount hda.img /mnt/mpam
    - $ sudo cp /path/to/edk2/Build/Shell/DEBUG_GCC49/AARCH64/Mpam.efi /mnt/mpam
    - $ sudo umount /mnt/mpam
2.  Load the image file to the secondary storage using a backdoor. The steps for this are beyond the scope of this document.
3.  Boot the system to UEFI shell.
4.  To determine the file system number of the secondary storage, execute 'map -r' command.
5.  Execute 'fsx' where 'x' is replaced by the number determined in step 4.
6.  To start the compliance tests, run the executable Mpam.efi with appropriate command arguments as follows: <br />

    Mpam.efi: Mpam.efi [-v &lt;verbosity&gt;] [-skip &lt;test_id&gt;] [-f &lt;filename&gt;]

    Options:

        -v      select which print level to use. Default is TEST
                1 - ERROR,  prints only error messages
                2 - WARN,   prints all warning and error messages
                3 - TEST,   prints all test, warning and error messages
                4 - DEBUG,  prints all debug, test, warning and error messages
                5 - INFO,   prints all types of messages
        -skip   omits the specified test case number execution
        -f      save shell command line output

## License
MPAM ACS is distributed under [Apache v2.0 License](LICENSE.md).


## Feedback, contributions and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, please send an email to "support-enterprise-acs@arm.com" with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. See GitHub documentation on how to raise pull requests. <br />


*Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.*
