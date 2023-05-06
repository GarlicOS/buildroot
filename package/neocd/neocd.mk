################################################################################
#
# neocd
#
################################################################################

NEOCD_VERSION = 2070f5258c9d3feee15962f9db8c8ef20072ece8
NEOCD_SITE = https://github.com/libretro/neocd_libretro.git
NEOCD_SITE_METHOD = git
NEOCD_LICENSE = LGPL-3.0
NEOCD_LICENSE_FILES = LICENSE.md
NEOCD_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define NEOCD_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(NEOCD_MAKE_OPTS)
endef

define NEOCD_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/neocd_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/neocd_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/neocd_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
