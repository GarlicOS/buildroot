################################################################################
#
# handy
#
################################################################################

HANDY_VERSION = 63db085af671bad2929078c55434623b7d4632a1
HANDY_SITE = https://github.com/libretro/libretro-handy.git
HANDY_SITE_METHOD = git
HANDY_LICENSE = Zlib
HANDY_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define HANDY_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(HANDY_MAKE_OPTS)
endef

define HANDY_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/handy_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/handy_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/handy_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
