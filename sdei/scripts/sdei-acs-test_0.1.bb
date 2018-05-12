LICENSE = "GPLv2"

inherit module

SRC_URI = " file://tests/ \
            file://val/  \
            file://Makefile \
	    file://COPYING \
            "
S = "${WORKDIR}"
LIC_FILES_CHKSUM = "file://COPYING;md5=2a944942e1496af1886903d274dedb13"


do_install_append () {
        cp ${S}/sdei_acs_test.o ${DEPLOY_DIR_IMAGE}/
}


