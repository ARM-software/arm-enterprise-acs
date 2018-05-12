
#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

DESCRIPTION = "Builds Uefi Application"
SECTION = "exampled"
LICENSE = "CLOSED"


FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
inherit deploy

SRC_URI =  " https://github.com/tianocore/edk2/archive/dbc85eb.zip;name=name2 \
             https://releases.linaro.org/components/toolchain/binaries/5.3-2016.05/aarch64-linux-gnu/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu.tar.xz;name=name3 \
             file://sdei/ \
	     file://compile.sh"
SRC_URI[name2.md5sum] = "00667c21c5f6109ffdaaec5e79f30771"
SRC_URI[name3.md5sum] = "24ac2e26f50f49f3043f281440b41bba"

do_unpack () {
  /usr/bin/unzip ${DL_DIR}/dbc85eb.zip -d ${S}/
  mv ${S}/edk2-dbc85eb993439a7006bb20091c1cc6de43d19e80/ ${S}/edk2/

  tar -xf ${DL_DIR}/gcc-linaro*.tar.xz
  mv gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/ ${S}/gcc-linaro/
  ls
}

do_configure () {
cp -r ${THISDIR}/files/sdei edk2/AppPkg/Applications/sdei-acs
sed -i '/LibraryClasses.common/ a\ SdeiValLib|AppPkg/Applications/sdei-acs/val/SdeiValLib.inf' edk2/ShellPkg/ShellPkg.dsc
sed -i '/LibraryClasses.common/ a\ SdeiPalLib|AppPkg/Applications/sdei-acs/platform/pal_uefi/SdeiPalLib.inf' edk2/ShellPkg/ShellPkg.dsc
sed -i '/Components/ a\ AppPkg/Applications/sdei-acs/uefi_app/SdeiAcs.inf' edk2/ShellPkg/ShellPkg.dsc
}

do_compile () {
   export GCC49_AARCH64_PREFIX=${S}/gcc-linaro/bin/aarch64-linux-gnu-
   /bin/bash ${THISDIR}/files/compile.sh ${S}
}


do_install () {
    echo "do_install: Copying SDEI files to destination ${D}"
    mkdir ${D}/sdei
	cp ${S}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/Sdei.efi ${D}/sdei/Sdei.efi
	cp ${S}/edk2/ShellBinPkg/UefiShell/AArch64/Shell.efi ${D}/sdei/Shell.efi
}


do_deploy () {
    echo "do_deploy: Deploying SDEI files to destination ${DEPLOYDIR}"
    install -d ${DEPLOYDIR}
    cp -r ${D}/sdei ${DEPLOYDIR}/sdei
}

addtask deploy before do_build after do_install

