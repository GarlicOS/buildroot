################################################################################
#
# mame2000
#
################################################################################

MAME2000_VERSION = 720b8ad4cbd76abd57b9aeced9ba541dc8476f7f
MAME2000_SITE = https://github.com/libretro/mame2000-libretro.git
MAME2000_SITE_METHOD = git
MAME2000_LICENSE = MAME
MAME2000_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define MAME2000_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(MAME2000_MAKE_OPTS)
endef

define MAME2000_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mame2000_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mame2000_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mame2000_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
