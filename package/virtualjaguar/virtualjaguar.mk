################################################################################
#
# virtualjaguar
#
################################################################################

VIRTUALJAGUAR_VERSION = 2cc06899b839639397b8b30384a191424b6f529d
VIRTUALJAGUAR_SITE = https://github.com/libretro/virtualjaguar-libretro.git
VIRTUALJAGUAR_SITE_METHOD = git
VIRTUALJAGUAR_LICENSE = GPL-3.0
VIRTUALJAGUAR_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define VIRTUALJAGUAR_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(VIRTUALJAGUAR_MAKE_OPTS)
endef

define VIRTUALJAGUAR_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/virtualjaguar_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/virtualjaguar_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/virtualjaguar_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
