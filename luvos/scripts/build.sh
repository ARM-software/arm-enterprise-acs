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

ALLOWED_FIRST_PARAM="sbbr sbsa sdei luv-live-image cleanall"
ALLOWED_SECOND_PARAM="cleanall"
NUMOFARGS=0;
MODULE=""
CLEANALLONLY=0

OUTPUT_FILE="$PWD/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img"

display_usage()
{
        echo
        echo "Usage: $0 [sbbr|sbsa|sdei|luv-live-image] [cleanall]"
        echo
}

validate_param()
{
	occurrence=`echo $ALLOWED_FIRST_PARAM | grep -w $1 | wc -l`
	if [ $occurrence -eq 0 ]; then
		echo "First Parameter $1 is not correct"
		display_usage;
		exit 1;
	fi
	MODULE=$1;
	NUMOFARGS=1;

	if [ $MODULE == "cleanall" ]; then
		CLEANALLONLY=1;
		return;
	fi
	if [ $# -eq 2 ]; then
		occurrence=`echo $ALLOWED_SECOND_PARAM | grep -w $2 | wc -l`
		if [ $occurrence -eq 0 ]; then
			echo "Second Parameter is not correct"
			display_usage;
			exit 1;
		fi
		NUMOFARGS=2;
	fi
}

if [ $# -gt 2 ]; then
	display_usage;
	exit;
fi

if [ $# -gt 0 ]; then
	validate_param $*
	shift;
	shift;
fi

cd $TOPDIR/luv
source oe-init-build-env

if [ $CLEANALLONLY -eq 1 ]; then
	echo
	echo -e "Are you sure to clean all modules? [Y/N]"
        read input
        if [ "${input}" == "Y" ]
        then
		rm -rf build
		bitbake -c cleanall sbbr
		bitbake -c cleanall sbsa
		bitbake -c cleanall sdei
		bitbake -c cleanall luv-live-image
		exit 0;
	else
		exit 0;
	fi
fi

echo "Building LuvOS Image with SBBR and SBSA for AARCH4 ..."
echo ""
echo "Default kernel command line parameters: 'systemd.log_target=null plymouth.ignore-serial-consoles debug ip=dhcp log_buf_len=1M efi=debug acpi=on crashkernel=256M earlycon uefi_debug'"
echo -n "Append parameters (press Enter for default):"
read ACS_CMDLINE_APPEND
export ACS_CMDLINE_APPEND
export BB_ENV_EXTRAWHITE="BB_ENV_EXTRAWHITE ACS_CMDLINE_APPEND"

if [ $NUMOFARGS -gt 0 ]; then
	if [ $NUMOFARGS -eq 1 ]; then
		#One parameter case
		echo -e "\nWarning: If the entire package is not built at least once before"
		echo -e "then building only '$MODULE' may fail."
		echo -e "Are you sure to continue? [Y/N]"
		read input
		if [ "${input}" == "Y" ]
		then
			echo
			echo "Executing with one parameter $0 $MODULE";
			bitbake $MODULE
			bitbake luv-live-image
		else
			exit 0
		fi
	else
		#two parameters case
		echo "bitbake -c cleanall $MODULE"
		bitbake -c cleanall $MODULE
	fi
else
	echo "Building all modules"
	bitbake sbbr
	bitbake sbsa
	bitbake sdei
	bitbake luv-live-image
fi

unset BB_ENV_EXTRAWHITE
unset ACS_CMDLINE_APPEND
if [ -f $OUTPUT_FILE ]; then
	echo "Built image can be found at $OUTPUT_FILE"
else
	echo "Build failed..."
fi
exit

