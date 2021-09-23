# Arm Enterprise ACS - Architecture Compliance Suite

## Architecture Compliance Suite
Architecture Compliance Suite (ACS) is used to ensure architectural compliance across different implementations of the architecture. Arm Enterprise ACS includes a set of examples of the invariant behaviors that are provided by a set of specifications for enterprise systems (For example: SBSA, SBBR, etc.), so that implementers can verify if these behaviours have been interpreted correctly. ACS is delivered with tests in source form along with a build script, the output of the build being a bootable Linux UEFI Validation (LUV) OS image that can run all tests required by these specifications.

Arm Enterprise ACS tests are available open source. The tests and the corresponding abstraction layers are available with an Apache v2 license allowing for external contribution.

In summary, the Arm Enterprise ACS product contains the following: <ol>
1. Scripts to build, construct, and run the test images. <br />
2. A bootable LUV OS image capable of running all tests. <br />
3. Documentation on running the tests. <br /> </ol>

These tests are split between UEFI and Linux (supported by corresponding kernel driver) applications that together determine whether an architectural implementation is compliant with the enterprise specifications. These tests are further described in detail.

## Release details
 - Code Quality: REL v3.1
 - **The latest pre-built release of ACS is available for download here: [v21.09_REL3.1](https://github.com/ARM-software/arm-enterprise-acs/tree/release/prebuilt_images/v21.09_REL3.1)**
 - The SBSA tests are written for version 6.0 of the SBSA specification.
 - The SBBR tests are written for version 1.0 of the BBR specification.
 - The compliance suite is not a substitute for design verification.
 - To review the ACS logs, Arm licensees can contact Arm directly through their partner managers.

## GitHub branch
- To pick up the release version of the code, checkout the release branch with the appropriate tag.
- To get the latest version of the code with bug fixes and new features, use the master branch.

## ACS build steps

### Prebuilt images
- Prebuilt images for each release are available in the prebuilt_images folder of the release branch. You can either choose to use these images or build your own image by following the steps below.
- To access the prebuilt_images, click this link : [prebuilt_images](https://github.com/ARM-software/arm-enterprise-acs/tree/release/prebuilt_images)
- If you choose to use the prebuilt image, skip the build steps and jump to the test suite execution section below.

### Prerequisites
Before starting the ACS build, ensure that the following requirements are met:
 - Ubuntu 18.04 LTS or Ubuntu 20.04 with at least 64GB of free disk space.
 - Must use Bash shell.
 - Build is supported on x86 and AArch64 machines.

<br />

Perform the following steps to start the ACS build:

1. Create a directory that is your workspace and `cd' into it. <br />
   $ mkdir &lt;work_dir&gt; && cd &lt;work_dir&gt; <br />
2. Clone the Arm Enterprise ACS source code. <br />
   $ git clone https://github.com/ARM-software/arm-enterprise-acs.git <br />
   $ cd arm-enterprise-acs <br />
3. Download and patch LUV OS source code. <br />
   $ ./acs_sync.sh <br />
4. Build LUV OS and test binaries. <br />
   $ ./luvos/scripts/build.sh

Note:<br />
- These build steps only target AArch64. <br />
- The build script provides the option to append kernel command-line parameters, if necessary. Press enter to continue with default parameters. <br />
- For build options to selectively sync and build particular modules, check this document : [build-options](https://github.com/ARM-software/arm-enterprise-acs/blob/master/docs/build-options.md). <br />

## Build output
The luv-live-image-gpt.img bootable image can be found in:
&lt;work_dir&gt;/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img<br />

This image comprises of two FAT file system partitions recognized by UEFI: <br />
- 'luv-results' <br />
  Stores logs and is used to install UEFI-SCT. (Approximate size: 120 MB) <br/>
- 'boot' <br />
  Contains bootable applications and test suites. (Approximate size: 60 MB)

The grub-efi-bootaa64.efi netbootable image can be found in:
&lt;work_dir&gt;/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/grub-efi-bootaa64.efi<br />

This image is built to be used in PXE Booting and automatic execution of tests
For more details please check this document :  [build-options](https://github.com/ARM-software/arm-enterprise-acs/blob/master/docs/pxeboot-instructions.md). <br />


For more information, see [Yocto Project](https://www.yoctoproject.org/documentation) and [LuvOS](https://github.com/01org/luv-yocto). <br />

## Test Suite Execution

Note: UEFI EDK2 setting for "Console Preference": The default is "Graphical". When that is selected, Linux output will go only to the graphical console (HDMI monitor). To force serial console output, you may change the "Console Preference" to "Serial".

### Verification of the luv-live-image on the Arm Neoverse N2 reference design (RD-N2)

#### Prerequisites
- If the system supports LPIs (Interrupt ID > 8192) then Firmware should support installation of handler for LPI interrupts.
    - If you are using edk2, change the ArmGic driver in the ArmPkg to support installation of handler for LPIs.
    - Add the following in \<path to RDN2 software stack\>/uefi/edk2/ArmPkg/Drivers/ArmGic/GicV3/ArmGicV3Dxe.c
>        - After [#define ARM_GIC_DEFAULT_PRIORITY  0x80]
>          +#define ARM_GIC_MAX_NUM_INTERRUPT 16384
>        - Change this in GicV3DxeInitialize function.
>          -mGicNumInterrupts      = ArmGicGetMaxNumInterrupts (mGicDistributorBase);
>          +mGicNumInterrupts      = ARM_GIC_MAX_NUM_INTERRUPT;

#### Follow the steps mentioned in [RD-N2 platform software user guide](https://gitlab.arm.com/arm-reference-solutions/arm-reference-solutions-docs/-/tree/master/docs/infra/rdn2) to obtain RD-N2 FVP.

### For software stack build instructions follow Busybox Boot link under Supported Features by RD-N2 platform software stack section in the same guide.

Note: RD-N2 should be built with the GIC Changes mentioned in Prerequisites.<br />
Note: sudo permission will be required by building software stack.<br />

1. Set the environment variable 'MODEL'
```
export MODEL=<absolute path to the RD-N2 FVP binary/FVP_RD_N2>
```
2. Launch the RD-N2 FVP with the pre-built image with the below command
```
cd /path to RD-N2_FVP platform software/model-scripts/rdinfra/platforms/rdn2
./run_model.sh -v /path-to-luv-live-image/luv-live-image-gpt.img
```
This will start the luv live image automation and run the test suites in sequence.


### Automation
The test suite execution can be automated or manual. Automated execution is the default execution method when no key is pressed during boot. <br />
Note: SBBR SCT tests are now included as part of automation. For information about running these tests, see section ‘SBBR SCT tests’ in this [document](sbbr/README.md). <br />
The execution varies depending on the test environment.
The next set of commands are an example of our typical run of the test suites.
Note that the File System Partition in your platform can vary. <br />

The live image boots to UEFI Shell. The different test applications can be run in following order:

1. [UEFI Shell application](sbsa/README.md) for SBSA compliance.
2. [SCT tests](sbbr/README.md) (if included in the build), for SBBR compliance.
3. [FWTS tests](sbbr/README.md) for SBBR compliance.
4. [OS tests](sbsa/README.md) for SBSA compliance.

## Baselines for Open Source Software in this release:

- [Linux UEFI Validation OS](https://github.com/intel/luv-yocto)
        - SHA: 73f995b61a7b1b856a082203cbeb744a3f21880d

- [Firmware Test Suite (FWTS) TAG: V21.08.00](http://kernel.ubuntu.com/git/hwe/fwts.git)
  Note: For improved FTWS test coverage, use release images of ACSv2.5 or higher

- [Server Base System Architecture (SBSA)](https://github.com/ARM-software/sbsa-acs) TAG: 1b3a37214fe6809e07e471f79d1ef856461bc803

- [UEFI Self Certification Tests (UEFI-SCT)](https://github.com/tianocore/edk2-test) TAG: f3b3456152dacf26ec0abaa0c21a9432cc176630
Note: UEFI-SCT is based on edk2-test release (tag:edk2-test-stable202108) which supports UEFI Specifications version 2.7
Refer to https://uefi.org/testtools for more details


## Security Implication
Arm Enterprise ACS test suite may run at higher privilege level. An attacker may utilize these tests as a means to elevate privilege which can potentially reveal the platform security assets. To prevent the leakage of secure information, it is strongly recommended that the ACS test suite is run only on development platforms. If it is run on production systems, the system should be scrubbed after running the test suite.

## Limitations

Validating the compliance of certain PCIe rules defined in the BSA specification require the PCIe end-point generate specific stimulus during the runtime of the test. Examples of such stimulus are  P2P, PASID, ATC, etc. The tests that requires these stimuli are grouped together in the exerciser module. The exerciser layer is an abstraction layer that enables the integration of hardware capable of generating such stimuli to the test framework.
The details of the hardware or Verification IP which enable these exerciser tests platform specific and are beyond the scope of this document.

The Live image does not allow customizations, hence, the exerciser module is not included in the Live image. To enable exerciser tests for greater coverage of PCIe rules, please refer to [SBSA](https://github.com/ARM-software/sbsa-acs) Or contact your Arm representative for details.


## License

Arm Enterprise ACS is distributed under Apache v2.0 License.

## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, please send an email to "support-enterprise-acs@arm.com" with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. For details, see "docs/Contributions.txt".

--------------

*Copyright (c) 2018-2021, Arm Limited and Contributors. All rights reserved.*
