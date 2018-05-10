SUMMARY = "Compiles sbsa val"
LICENSE = "Apachev2"
LIC_FILES_CHKSUM = "file://COPYING;md5=ebefc2951b134f1525329b63257fa018"
inherit module

SRC_URI = " file://Makefile \
            file://include/ \
            file://src/ \
	    file://COPYING \
            "
S = "${WORKDIR}"


do_install_append () {
        cp "${S}"/sbsa_acs_val.o ${DEPLOY_DIR_IMAGE}/

}
