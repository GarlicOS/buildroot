################################################################################
#
# fceumm
#
################################################################################

FCEUMM_VERSION = 398356bc34db45a1b39f6bce7219a4cf1afa2328
FCEUMM_SITE = https://github.com/libretro/libretro-fceumm.git
FCEUMM_SITE_METHOD = git
FCEUMM_LICENSE = GPL-2.0
FCEUMM_LICENSE_FILES = Copying
FCEUMM_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FCEUMM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(FCEUMM_MAKE_OPTS)
endef

define FCEUMM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/fceumm_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/fceumm_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/fceumm_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
