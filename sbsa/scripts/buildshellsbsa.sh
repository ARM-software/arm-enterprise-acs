#!/bin/bash
# Copyright (c) 2019, ARM Limited or its affiliates. All rights reserved.
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

TOPDIR=$1
NETBOOT_OUTPUT_FILE_PATH="$TOPDIR/luv/build/tmp/deploy/images/qemuarm64"

WORKDIR=`bitbake -e sbsa | grep ^WORKDIR=  | awk -F'=' '{print $2}' | sed 's/\"//g'`
echo "WORKDIR is $WORKDIR"
if [ -z $WORKDIR ]; then
	echo "Error: WORKDIR could not be set. Cannot proceed!"
	exit 1;
fi

echo "TOPDIR is $TOPDIR"


toolsdir=`ls -d ${WORKDIR}/gcc*aarch64*`

if [ -z $toolsdir ]; then
	echo "Error: There is no tools directory in ${WORKDIR}. Cannot proceed!"
	exit 1;
fi

TOOLSPATH=$toolsdir;

echo "TOOLS PATH is $TOOLSPATH"


#Backup the existing Shell.efi
#This may not be needed
#mv -f ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/Shell.efi ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/BackupShell.efi 


####ApplyPatch to change the following files
#${WORKDIR}/edk2/ShellPkg/Application/Shell/Shell.c
#${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.c
#${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.h
#${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf

cd ${WORKDIR}/edk2
echo "Apply Shell Sbsa Patch"
git apply --ignore-space-change --ignore-whitespace $TOPDIR/sbsa/patches/ShellSbsa.patch



##Direct copy
rm -f ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/SbsaAvsMain.c
cp ${WORKDIR}/git/uefi_app/SbsaAvsMain.c ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/

rm -f ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/SbsaAvs.h
cp ${WORKDIR}/git/uefi_app/SbsaAvs.h ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/

rm -f ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/SbsaAvs.inf
cp ${WORKDIR}/git/uefi_app/SbsaAvs.inf ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/



rm -rf ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/test_pool
cp -r ${WORKDIR}/git/test_pool ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/

rm -rf ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/val
cp -r ${WORKDIR}/git/val ${WORKDIR}/edk2/ShellPkg/Library/UefiShellLevel3CommandsLib/

cd ${WORKDIR}/edk2/

set --
source edksetup.sh

MACHINE=`uname -m`
echo "Architecture Detected : $MACHINE"
if [ $MACHINE = "aarch64" ]; then
    export GCC49_AARCH64_PREFIX=/usr/bin/
else
    export GCC49_AARCH64_PREFIX=${TOOLSPATH}/bin/aarch64-linux-gnu-
fi

echo "GCC49_AARCH64_PREFIX = $GCC49_AARCH64_PREFIX"

cd ${WORKDIR}/edk2/

#Actual Build
build -a AARCH64 -t GCC49 -p ShellPkg/ShellPkg.dsc -m ShellPkg/Application/Shell/Shell.inf 


mv ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/Shell.efi ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/ShellSbsa.efi 

####RevokePatch
cd ${WORKDIR}/edk2
echo "Revoke Shell Sbsa Patch"
git apply -R --ignore-space-change --ignore-whitespace $TOPDIR/sbsa/patches/ShellSbsa.patch


if [ -f ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/ShellSbsa.efi ]; then
	echo "ShellSbsa Build Success"
else
	echo "ShellSbsa Build Failed. Please check compilation logs for details"
	exit 1;
fi

echo "Copying ShellSbsa.efi to $NETBOOT_OUTPUT_FILE_PATH"
chmod +x ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/ShellSbsa.efi
cp ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/ShellPkg/Application/Shell/Shell/DEBUG/ShellSbsa.efi $NETBOOT_OUTPUT_FILE_PATH

exit 0;
