
# Software Delegated Exception Interface - Architecture Compliance Suite

## Software Delegated Exception Interface
**Software Delegated Exception Interface** (SDEI) Platform Design Document provides a mechanism for registering and servicing system events from system firmware.

For more information, download the [SDEI Platform Design Document](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0054a/index.html).

## Release details
 - Code Quality: Beta v0.6
 - Code Completed: 48 out of 52.
 - The tests are written for version 1.0 of the SDEI Platform Design Document.

## SDEI compliance source

The SDEI ACS tests compliance as a client to SDEI dispatcher running in EL3 as part of ARM-TF. The client application runs in EL2 (UEFI application) and EL1 (Linux kernel module/userspace application).

## Downloading SDEI ACS

SDEI ACS is a sub-directory in arm-enterprise-acs repository.

$ git clone https://github.com/ARM-software/arm-enterprise-acs.git<br/>
$ cd arm-enterprise-acs/sdei

## Building SDEI ACS
### UEFI application
#### Prerequisites

Before starting the ACS build, ensure that the following requirements are met.

- Any mainstream Linux based OS distribution.
- git clone EDK2 tree.
- Install GCC 6.3 or later toolchain for Linux from [here](https://releases.linaro.org/components/toolchain/binaries).
- Install the build prerequisite packages to build EDK2. The details of the packages are beyond the scope of this document.

#### Build Steps
<ol>
$ cd /path/to/arm-enterprise-acs/sdei<br/>
$ ./scripts/build_uefi.sh
</ol>

#### Build Output

The following output file is created in /path/to/arm-enterprise-acs/sdei/workspace/output/:

- Sdei.efi

### Linux kernel module and user space application
#### Build Steps
<ol>
$ cd /path/to/arm-enterprise-acs/sdei<br/>
$ ./scripts/build_linux.sh
</ol>

#### Build Output
The following output files are created in in /path/to/arm-enterprise-acs/sdei/workspace/output/:

- sdei_acs.ko
- sdei
- Image


## Test suite execution in UEFI

### Post-Silicon

On a system where a USB port is available and functional, perform the following steps:

1. Copy 'Sdei.efi' to a USB Flash drive.
2. Plug in the USB Flash drive to one of the functional USB ports on the system.
3. Boot the system to UEFI shell.
4. To determine the file system number of the plugged in USB drive, execute 'map -r' command.
5. Type 'fsx' where 'x' is replaced by the number determined in step 4.
6. To start the compliance tests, run the executable Sdei.efi with the appropriate arguments.

### Emulation environment with secondary storage

On an emulation environment with secondary storage, perform the following steps:

1. Create an image file which contains the 'Sdei.efi' file. For Example: <br/>
$ mkfs.vfat -C -n HD0 hda.img 31457280 <br/>
$ sudo mount hda.img /mnt/sdei <br/>
$ cd /path/to/arm-enterprise-acs/sdei/workspace/output/ <br/>
$ sudo cp Sdei.efi /mnt/sdei/ <br/>
$ sudo umount /mnt/sdei
2. Load the image file to the secondary storage using a backdoor. The steps followed to load the image file are Emulation environment specific and beyond the scope of this document.
3. Boot the system to UEFI shell.
4. To determine the file system number of the secondary storage, execute 'map -r' command.
5. Type 'fsx' where 'x' is replaced by the number determined in step 4.
6. To start the compliance tests, run the executable Sdei.efi with the appropriate arguments.

### Emulation environment without secondary storage

On an Emulation platform where secondary storage is not available, perform the following steps:

1. Add the path to 'Sdei.efi' file in the UEFI FD file.
2. Build UEFI image including the UEFI Shell.
3. Boot the system to UEFI shell.
4. Run the executable 'Sdei.efi' to start the compliance tests.

## Test suite execution in Linux

### Post-Silicon

On a system where a USB port is available and functional, perform the following steps:

1. Copy kernel module <i>sdei_acs.ko</i>, user space application <i>sdei</i> and kernel binary <i>Image</i> to a USB flash drive.
2. Boot kernel binary <i>Image</i> using platform specific kernel arguments.
3. On linux shell, mount the USB flash drive. For example:<br/>#mount /dev/sdb /mnt
4. Insert sdei kernel module and run sdei application.<br/>#cd /mnt<br/>#insmod sdei_acs.ko<br/>#./sdei

### Emulation environment with secondary storage

On an emulation environment with secondary storage, perform the following steps:

1. Create an image file which contains the 'Sdei.efi' file. For Example: <br/>
$ mkfs.vfat -C -n HD0 hda.img 31457280 <br/>
$ sudo mount hda.img /mnt/sdei <br/>
$ cd /path/to/arm-enterprise-acs/sdei/workspace/output/ <br/>
$ sudo cp sdei_acs.ko sdei Image /mnt/sdei <br/>
$ sudo umount /mnt/sdei
2. Load the image file to the secondary storage using a backdoor. The steps followed to load the image file are Emulation environment specific and beyond the scope of this document.
3. Boot kernel binary <i>Image</i> using platform specific kernel arguments.
4. On linux shell, mount the secondary storage. For example:<br/>#mount /dev/vda /mnt
5. Insert sdei kernel module and run sdei application.<br/>#cd /mnt<br/>#insmod sdei_acs.ko<br/>#./sdei

## Application arguments

Command line arguments are similar for uefi and linux applications, with some exceptions.

### UEFI

Shell> Sdei.efi [-v &lt;verbosity&gt;] [-skip &lt;test_id&gt;] [-f &lt;filename&gt;]


### Linux
\# sdei [-v &lt;verbosity&gt;] [-skip &lt;test_id&gt;]


#### -v
Choose the verbosity level.

- 1 - ERROR
- 2 - WARN and ERROR
- 3 - TEST and above
- 4 - DEBUG and above
- 5 - INFO and above
- 6 - KERNEL dump kernel prints during tests (Only for linux application)

#### -skip
Overrides the suite to skip the execution of a particular
test. For example, <i>-skip 10</i> skips test 10.

#### -f (Only for UEFI application)
Save the test output into a file in secondary storage. For example <i>-f sdei.log</i> creates a file sdei.log with test output.


### UEFI example

Shell> Sdei.efi -v 5 -skip 15,20,30 -f sdei_uefi.log

Runs SDEI ACS with verbosity INFO, skips test 15, 20 and 30 and saves the test results in <i>sdei_uefi.log</i>.

### Linux example

\# sdei -v 1 -skip 10,20

Runs SDEI ACS with verbosity ERROR, skips test 10 and 20.

## Interrupt numbers used in SDEI testing

SPI interrupts: 230, 231
PPI interrupts: 18
SGI interrupts: 5
 
##SDEI compliance - Known Issues

- Test #30 fails in linux. This is under debug.
- Test #45, #47 and #48 are run only in UEFI Shell application and is skipped when run in Linux.
- Running the SDEI ACS once is sufficient for compliance. There are stability issues when the ACS application is run multiple times without rebooting the system.
