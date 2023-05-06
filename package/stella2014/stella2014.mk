################################################################################
#
# stella2014
#
################################################################################

STELLA2014_VERSION = 8ab051edd4816f33a5631d230d54059eeed52c5f
STELLA2014_SITE = https://github.com/libretro/stella2014-libretro.git
STELLA2014_SITE_METHOD = git
STELLA2014_LICENSE = GPL-2.0
STELLA2014_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define STELLA2014_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(STELLA2014_MAKE_OPTS)
endef

define STELLA2014_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/stella2014_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/stella2014_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/stella2014_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
