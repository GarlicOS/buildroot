################################################################################
#
# vemulator
#
################################################################################

VEMULATOR_VERSION = ff9c39714fe64960b4050c6884c70c24e63de4fd
VEMULATOR_SITE = https://github.com/libretro/vemulator-libretro.git
VEMULATOR_SITE_METHOD = git
VEMULATOR_LICENSE = GPL-3.0
VEMULATOR_LICENSE_FILES = COPYING
VEMULATOR_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define VEMULATOR_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(VEMULATOR_MAKE_OPTS)
endef

define VEMULATOR_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vemulator_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vemulator_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/vemulator_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
