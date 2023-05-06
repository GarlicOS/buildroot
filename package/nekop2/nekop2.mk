################################################################################
#
# nekop2
#
################################################################################

NEKOP2_VERSION = cd389b3490d5048873c40edd949a1b0631e0606d
NEKOP2_SITE = https://github.com/libretro/libretro-meowPC98.git
NEKOP2_SITE_METHOD = git
NEKOP2_LICENSE = MIT
NEKOP2_LICENSE_FILES = LICENSE
NEKOP2_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define NEKOP2_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/libretro -f Makefile.libretro $(NEKOP2_MAKE_OPTS)
endef

define NEKOP2_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/nekop2_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/nekop2_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/libretro/nekop2_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
