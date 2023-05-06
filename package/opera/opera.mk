################################################################################
#
# opera
#
################################################################################

OPERA_VERSION = 8a49bb8877611037438aeb857cb182f41ee0e3a1
OPERA_SITE = https://github.com/libretro/opera-libretro.git
OPERA_SITE_METHOD = git
OPERA_LICENSE = LGPL/Non-commercial
OPERA_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define OPERA_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(OPERA_MAKE_OPTS)
endef

define OPERA_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/opera_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/opera_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/opera_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
