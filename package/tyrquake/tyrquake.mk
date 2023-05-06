################################################################################
#
# tyrquake
#
################################################################################

TYRQUAKE_VERSION = b3fcc43e57543ef499df91b5c5d931680addf070
TYRQUAKE_SITE = https://github.com/libretro/tyrquake.git
TYRQUAKE_SITE_METHOD = git
TYRQUAKE_LICENSE = GPL-2.0
TYRQUAKE_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define TYRQUAKE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(TYRQUAKE_MAKE_OPTS)
endef

define TYRQUAKE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/tyrquake_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/tyrquake_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/tyrquake_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
