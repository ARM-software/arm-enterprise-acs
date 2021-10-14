#!/bin/bash
# Copyright (c) 2017-2019, ARM Limited or its affiliates. All rights reserved.
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

set -e
export GIT_SSL_NO_VERIFY=1
TOPDIR=`pwd`

#This parameter can be set to 0 if luv-netboot image is not needed in full build
NEED_NETBOOT_IMAGE=1

ALLOWED_FIRST_PARAM="sbbr sbsa sdei luv-live-image luv-netboot-image cleanall"
ALLOWED_SECOND_PARAM="cleanall"
NUMOFARGS=0;
MODULE=""
CLEANALLONLY=0

OUTPUT_FILE="$PWD/luv/build/tmp/deploy/images/qemuarm64/luv-live-image-gpt.img"
PATH_NETBOOT="$PWD/luv/build/tmp/deploy/images/qemuarm64"
NETBOOT_OUTPUT_FILE="$PWD/luv/build/tmp/deploy/images/qemuarm64/grub-efi-bootaa64.efi"
SHELLSBSA_FILE="$PWD/luv/build/tmp/deploy/images/qemuarm64/ShellSbsa.efi"
LOCAL_CONF_FILE="$TOPDIR/luv/build/conf/local.conf"

display_usage()
{
        echo
        echo "Usage: $0 [sbbr|sbsa|sdei|luv-live-image|luv-netboot-image] [cleanall]"
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

build_shell_sbsa()
{
	$TOPDIR/sbsa/scripts/buildshellsbsa.sh $TOPDIR

}


build_netboot_image()
{
	if [ ! -f $LOCAL_CONF_FILE ]; then
		echo "$LOCAL_CONF_FILE is not found. Cannot build luv-netboot-image"
		return;
	fi

	cp $LOCAL_CONF_FILE ${LOCAL_CONF_FILE}.luv-live-image
	#Change local.conf
	sed -i 's/^[ \t]*DISTRO.*=.*\".*\"/DISTRO = "luv-netboot"/g' ${LOCAL_CONF_FILE}

	#Apply netboot patch
	echo "Applying luv_netboot patch"
	cd $TOPDIR/luv
	git apply $TOPDIR/luvos/patches/luvos_netboot.patch
        bitbake luv-netboot-image

        #Revert to previous state
	cp ${LOCAL_CONF_FILE}.luv-live-image $LOCAL_CONF_FILE
	cp $LOCAL_CONF_FILE ${LOCAL_CONF_FILE}.luv-netboot-image

	#revoke the netboot patch
	echo "Revoking the netboot patch"
	cd $TOPDIR/luv
	git apply -R $TOPDIR/luvos/patches/luvos_netboot.patch

	echo "Building netboot image is completed. The image can be found at $NETBOOT_OUTPUT_FILE"

	echo "Creating PXE boot Package"
	if [ ! -f $SHELLSBSA_FILE ]; then
		echo "$SHELLSBSA_FILE is not found. Cannot create PXE Boot Package "
		return 1;
	fi

	cd $PATH_NETBOOT
	mkdir -p EFI/BOOT/sbsa
	cp $SHELLSBSA_FILE $PATH_NETBOOT/EFI/BOOT/sbsa
	cp $TOPDIR/luvos/scripts/modify_luv_params_efi.py $PATH_NETBOOT

	tar cvf PXEBOOT.tar grub-efi-bootaa64.efi modify_luv_params_efi.py EFI
	gzip -f PXEBOOT.tar

	if [ ! -f $PATH_NETBOOT/PXEBOOT.tar.gz ]; then
		echo "Creating PXE Boot Package Failed!"
		return 1;
	fi

	echo "PXE Boot Package is created at $PATH_NETBOOT/PXEBOOT.tar.gz"
	cd $TOPDIR/luv
	return 0
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
		bitbake -c cleanall luv-netboot-image
		exit 0;
	else
		exit 0;
	fi
fi

echo "Building LuvOS Image with SBBR and SBSA for AARCH64 ..."
echo ""
echo "Default kernel command line parameters: 'systemd.log_target=null plymouth.ignore-serial-consoles debug ip=dhcp log_buf_len=1M efi=debug acpi=on crashkernel=256M earlycon uefi_debug'"
if [ -n "$TERM" ] && [ "$TERM" != "dumb" ]; then
	echo -n "Append parameters (press Enter for default):"
	read ACS_CMDLINE_APPEND
	export ACS_CMDLINE_APPEND
	export BB_ENV_EXTRAWHITE="BB_ENV_EXTRAWHITE ACS_CMDLINE_APPEND"
fi

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

			if [ "${MODULE}" == "luv-netboot-image" ]; then
				build_shell_sbsa
				build_netboot_image
				exit 0
			fi

			bitbake $MODULE
			if [ "${MODULE}" != "luv-live-image" ]
			then
				bitbake luv-live-image
			fi
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
	if [ $? -eq 0 ] && [ $NEED_NETBOOT_IMAGE -eq 1 ]; then
		build_shell_sbsa
		build_netboot_image
	fi

fi

unset BB_ENV_EXTRAWHITE
unset ACS_CMDLINE_APPEND
if [ -f $OUTPUT_FILE ]; then
	echo "Built image can be found at $OUTPUT_FILE"
else
	echo "Build failed..."
	exit 1;
fi

exit 0
