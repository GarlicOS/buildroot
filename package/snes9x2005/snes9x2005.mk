################################################################################
#
# snes9x2005
#
################################################################################

SNES9X2005_VERSION = fd45b0e055bce6cff3acde77414558784e93e7d0
SNES9X2005_SITE = https://github.com/libretro/snes9x2005.git
SNES9X2005_SITE_METHOD = git
SNES9X2005_LICENSE = Non-commercial
SNES9X2005_LICENSE_FILES = copyright
SNES9X2005_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SNES9X2005_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(SNES9X2005_MAKE_OPTS)
endef

define SNES9X2005_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/snes9x2005_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/snes9x2005_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/snes9x2005_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
