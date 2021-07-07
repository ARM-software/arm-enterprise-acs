# Server Base System Architecture - Architecture Compliance Suite
SBSA specification outlines various system architecture features and software stack functionality that operating systems, hypervisors, and firmware can rely on.
SBSA test suites check for compliance against the SBSA specification. For source code, refer to [SBSA ACS repository](https://github.com/ARM-software/sbsa-acs). The tests are delivered through two runtime executable environments:
 - UEFI Shell SBSA tests
 - OS SBSA tests

## UEFI Shell SBSA tests
These tests are written on top of Validation Abstraction Layer (VAL) and Platform Abstraction Layer (PAL).

The abstraction layers provide platform information and runtime environment to enable execution of the tests.
In the present release, PAL is written on top of UEFI and ARM Trusted Firmware. For the OS tests, it is written on top of Linux kernel.
Partners can also write their own abstraction layer implementations to allow SBSA tests to be run in other environments, for example, as raw workload on an RTL simulation.

### Running UEFI Shell tests
 Enter the following commands to run the SBSA test on UEFI:

Shell>FS2:<br/>
FS2:>FS3:\EFI\BOOT\sbsa\sbsa.nsh

If any failures are encountered, see [SBSA User Guide](https://github.com/ARM-software/sbsa-acs/blob/master/docs/Arm_SBSA_Architecture_Compliance_User_Guide.pdf) for debug options.
Power reset the system after completion of this test and continue with the next step. <br />

Note:

- Here FS2: is assumed to be the 'luv-results' partition, and FS3: the 'boot' partition.
- These commands are applicable only for the first time when the image is executed to install the SCT tests.


## OS SBSA tests
Execution of some SBSA tests requires an OS environment.
This is particularly true for IO tests.
Currently, LUV OS is used. However, other OS images could be considered in future.

### Running OS tests
On Linux shell, enter the following command:

\#sbsa <br />

## Debug Information
SBSA source directories can be found at the following paths:

- SBSA UEFI application source at /path/to/arm-enterprise-acs/luv/build/tmp/work/aarch64-oe-linux/sbsa/1.0+gitAUTOINC+&lt;commit-id>-r0/git/
- SBSA Linux user application source at /path/to/arm-enterprise-acs/luv/meta-luv/recipes-utils/sbsa-acs-app/sbsa-acs-app/
- SBSA Linux kernel module source at /path/to/arm-enterprise-acs/luv/meta-luv/recipes-core/sbsa-acs-[drv|val|pal|test]/files/

To compile and test changes in the above source code, follow these steps:

- cd /path/to/arm-enterprise-acs/luv
- source oe-init-build-env

For UEFI application,

- bitbake sbsa -f -c compile
- bitbake sbsa

For linux user application,

- bitbake sbsa-acs-app -f -c compile
- bitbake sbsa-acs-app

For linux kernel module,

- bitbake sbsa-acs-drv -f -c compile
- bitbake sbsa-acs-drv

After this, run the following command to create an updated luv live image:
- bitbake luv-live-image


## Validation

Tests run on SGI-575 Reference Platforms

         i. UEFI Shell based tests
        ii. OS based tests built on top of Linux kernel

