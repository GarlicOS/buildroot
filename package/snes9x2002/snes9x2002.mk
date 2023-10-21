################################################################################
#
# snes9x2002
#
################################################################################

SNES9X2002_VERSION = 540baad622d9833bba7e0696193cb06f5f02f564
SNES9X2002_SITE = https://github.com/libretro/snes9x2002.git
SNES9X2002_SITE_METHOD = git
SNES9X2002_LICENSE = Non-commercial
SNES9X2002_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SNES9X2002_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(SNES9X2002_MAKE_OPTS)
endef

define SNES9X2002_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/snes9x2002_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/snes9x2002_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/snes9x2002_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
