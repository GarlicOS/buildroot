################################################################################
#
# gearsystem
#
################################################################################

GEARSYSTEM_VERSION = d9454402f323713e4a40a6b222fc3b9232773b94
GEARSYSTEM_SITE = https://github.com/drhelius/Gearsystem.git
GEARSYSTEM_SITE_METHOD = git
GEARSYSTEM_LICENSE = GPL-3.0
GEARSYSTEM_LICENSE_FILES = LICENSE
GEARSYSTEM_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define GEARSYSTEM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/platforms/libretro $(GEARSYSTEM_MAKE_OPTS)
endef

define GEARSYSTEM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/gearsystem_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/gearsystem_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/platforms/libretro/gearsystem_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
