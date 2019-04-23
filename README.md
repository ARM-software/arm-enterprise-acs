# Arm Enterprise ACS - Architecture Compliance Suite

## Architecture Compliance Suite
Architecture Compliance Suite (ACS) is used to ensure architectural compliance across different implementations of the architecture. Arm Enterprise ACS includes a set of examples of the invariant behaviours that are provided by a set of specifications for enterprise systems (e.g. SBSA, SBBR, etc.), so that implementers can verify if these behaviours have been interpreted correctly. ACS is delivered with tests in source form along with a build script, the output of the build being a bootable Linux UEFI Validation (LUV) OS image that can run all tests required by these specifications.

Arm Enterprise ACS tests are available open source. The tests and the corresponding abstraction layers are available with an Apache v2 license allowing for external contribution.

In summary, the Arm Enterprise ACS product contains the following: <ol>
1. Scripts to build, construct, and run the test images. <br />
2. A bootable LUV OS image capable of running all tests. <br />
3. Documentation on running the tests. <br /> </ol>

These tests are split between UEFI and Linux (supported by corresponding kernel driver) applications that together determine whether an architectural implementation is compliant with the enterprise specifications. These tests are further described in detail.

## Release details
 - Code Quality: REL v2.2
 - The SBSA tests are written for version 5.0 of the SBSA specification.
 - The SBBR tests are written for version 1.1 of the SBBR specification.
 - The compliance suite is not a substitute for design verification.
 - To review the ACS logs, Arm licensees can contact Arm directly through their partner managers.

## GitHub branch
- To pick up the release version of the code, checkout the release branch with the appropriate tag.
- To get the latest version of the code with bug fixes and new features, use the master branch.

## ACS build steps

### Prebuilt images
- Prebuilt images for each release are available in the prebuilt_images folder of the release branch. You can choose to use these images or build your own image by following the steps below.
- If you choose to use the prebuilt image, skip the build steps and jump to the test suite execution section below.

### Prerequisites
Before starting the ACS build, ensure that the following requirements are met:
 - Ubuntu 16.04 LTS with at least 64GB of free disk space.
 - Must use Bash shell.

Note : Windows build steps will be provided in the future releases.
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
- For build options to selectively sync and build particular modules, please check this document : [build-options](https://github.com/ARM-software/arm-enterprise-acs/blob/master/docs/build-options.md). <br />

## Build output
The luv-live-image-gpt.img bootable image can be found in:
&lt;work_dir&gt;/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img<br />

This image comprises of two FAT file system partitions recognized by UEFI: <br />
- 'luv-results' <br />
  Stores logs and is used to install UEFI-SCT. (Approximate size: 120 MB) <br/>
- 'boot' <br />
  Contains bootable applications and test suites. (Approximate size: 60 MB)

For more information, see [Yocto Project](https://www.yoctoproject.org/documentation) and [LuvOS](https://github.com/01org/luv-yocto). <br />

## Test Suite Execution

### Juno Reference Platform

Follow the instructions [here](https://community.arm.com/docs/DOC-10804) to install an EDK2 (UEFI) prebuilt configuration on your Juno board.
For additional information, see the FAQs and tutorials [here](https://community.arm.com/groups/arm-development-platforms) or contact [juno-support@arm.com](mailto:juno-support@arm.com).


After installing the EDK2 prebuilt configuration on your Juno board, follow these steps:

1. Burn the LUV OS bootable image to a USB stick: <br />
$ lsblk <br />
$ sudo dd if=/path/to/luv-live-image-gpt.img of=/dev/sdX <br />
$ sync <br />
Note: Replace '/dev/sdX' with the handle corresponding to your
  USB stick as identified by the `lsblk' command.
2. Insert the USB stick into one of the Juno's rear USB ports.
3. Power cycle the Juno.

### Fixed Virtual Platform (FVP) environment

The steps for running the Arm Enterprise ACS on an FVP are the
same as those for running on Juno but with a few exceptions:

- Follow the different instructions [here](https://community.arm.com/dev-platforms/b/documents/posts/using-linaros-deliverables-on-an-fvp) to install an EDK2 (UEFI) prebuilt configuration on your FVP.
- Modify 'run_model.sh' to add a model command argument that
  loads 'luv-live-image-gpt.img' as a virtual disk image. For example,
  if running on the AEMv8-A Base Platform FVP, add

    `bp.virtioblockdevice.image path=<work_dir>/arm-enterprise- acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img'

    to your model options. <br />
Or, <br />
To launch the FVP model with script ‘run_model.sh’ that supports -v option for virtual disk image, use the following command:

    $ ./run_model.sh -v &lt;work_dir>/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img

### Automation
The test suite execution can be automated or manual. Automated execution is the default execution method when no key is pressed during boot. <br />
Note: SBBR SCT tests are not included as part of automation. For information about running these tests, see section ‘SBBR SCT tests’ in this [document](sbbr/README.md). <br />
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

- [Firmware Test Suite (FWTS) TAG: V18.02.00](http://kernel.ubuntu.com/git/hwe/fwts.git)

- [Server Base System Architecture (SBSA)](https://github.com/ARM-software/sbsa-acs) TAG: <>

- [UEFI Self Certification Tests (UEFI-SCT)](https://github.com/UEFI/UEFI-SCT) TAG: c78ea66cb114390e8dd8de922bdf4ff3e9770f8c


## License

Arm Enterprise ACS is distributed under Apache v2.0 License.

## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, please send an email to "support-enterprise-acs@arm.com" with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. For details, see "docs/Contributions.txt".
