################################################################################
#
# snes9x
#
################################################################################

SNES9X_VERSION = 05a94e4370ef9b23540cd138a0d4cf0934755b35
SNES9X_SITE = https://github.com/libretro/snes9x.git
SNES9X_SITE_METHOD = git
SNES9X_LICENSE = Non-commercial
SNES9X_LICENSE_FILES = LICENSE
SNES9X_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SNES9X_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/libretro $(SNES9X_MAKE_OPTS)
endef

define SNES9X_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/snes9x_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/snes9x_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/libretro/snes9x_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
