################################################################################
#
# snes9x2010
#
################################################################################

SNES9X2010_VERSION = d8b10c4cd7606ed58f9c562864c986bc960faaaf
SNES9X2010_SITE = https://github.com/libretro/snes9x2010.git
SNES9X2010_SITE_METHOD = git
SNES9X2010_LICENSE = Non-commercial
SNES9X2010_LICENSE_FILES = LICENSE.txt
SNES9X2010_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SNES9X2010_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(SNES9X2010_MAKE_OPTS)
endef

define SNES9X2010_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/snes9x2010_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/snes9x2010_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/snes9x2010_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
