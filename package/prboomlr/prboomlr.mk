################################################################################
#
# prboomlr
#
################################################################################

PRBOOMLR_VERSION = 47a95e921e9f1e5928730b7369d14129769fa475
PRBOOMLR_SITE = https://github.com/libretro/libretro-prboom.git
PRBOOMLR_SITE_METHOD = git
PRBOOMLR_LICENSE = GPL-2.0
PRBOOMLR_LICENSE_FILES = COPYING
PRBOOMLR_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define PRBOOMLR_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(PRBOOMLR_MAKE_OPTS)
endef

define PRBOOMLR_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/prboom_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/prboom_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/prboom_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
