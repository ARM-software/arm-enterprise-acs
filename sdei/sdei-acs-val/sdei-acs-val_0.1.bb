
SUMMARY = "Compiles val"
LICENSE = "Apachev2"
LIC_FILES_CHKSUM = "file://COPYING;md5=2a944942e1496af1886903d274dedb13"
inherit module

SRC_URI = " file://Makefile \
            file://include/ \
            file://src/ \
            file://test_pool/ \
            file://COPYING \
            "
S = "${WORKDIR}"


do_install_append () {
        cp "${S}"/sdei_acs_val.o ${DEPLOY_DIR_IMAGE}/

}

