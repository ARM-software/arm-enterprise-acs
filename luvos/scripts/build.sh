#!/bin/bash
# Copyright (c) 2017, ARM Limited or its affiliates. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TOPDIR=`pwd`

OUTPUT_FILE="$PWD/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img"
cd $TOPDIR/luv

echo "Building LuvOS Image with SBBR and SBSA for AARCH4 ..."
echo "UEFI-SCT source is available only to members of UEFI Forum."
echo "To continue without building UEFI-SCT "
read -p "Enter [yes(default)/no]: " SCTOPTIONAL
SCTOPTIONAL=${SCTOPTIONAL:-yes}
export SCTOPTIONAL
if [ "$SCTOPTIONAL" = "no" ]
then
	echo -n User:
	read SCTUSERNAME
	echo -n Password:
	read -s SCTPASSWORD
	export SCTUSERNAME
	export SCTPASSWORD
else
	echo "Continue without building UEFI-SCT..."
fi
echo ""
echo "Default kernel command line parameters: 'systemd.log_target=null plymouth.ignore-serial-consoles debug ip=dhcp log_buf_len=1M efi=debug acpi=on crashkernel=256M earlycon uefi_debug'"
echo -n "Append parameters (press Enter for default):"
read ACS_CMDLINE_APPEND
export ACS_CMDLINE_APPEND
rm -rf build
source oe-init-build-env
export BB_ENV_EXTRAWHITE="BB_ENV_EXTRAWHITE SCTOPTIONAL SCTUSERNAME SCTPASSWORD ACS_CMDLINE_APPEND"
if [ "$SCTOPTIONAL" = "no" ]
then
	bitbake -c cleanall sbbr
fi
bitbake -c cleanall sbsa
bitbake -c cleanall sdei
bitbake -c cleanall luv-live-image
if [ "$SCTOPTIONAL" = "no" ]
then
	bitbake sbbr
fi
bitbake sbsa
bitbake sdei
bitbake luv-live-image
unset BB_ENV_EXTRAWHITE
unset SCTOPTIONAL
unset SCTPASSWORD
unset SCTUSERNAME
unset ACS_CMDLINE_APPEND
if [ -f $OUTPUT_FILE ]; then
	echo "Built image can be found at $OUTPUT_FILE"
else
	echo "Build failed..."
fi
exit
