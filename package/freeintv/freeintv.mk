################################################################################
#
# freeintv
#
################################################################################

FREEINTV_VERSION = 85bf25a39a34bbc39fe36677175d87c2b597dbe7
FREEINTV_SITE = https://github.com/libretro/FreeIntv.git
FREEINTV_SITE_METHOD = git
FREEINTV_LICENSE = GPL-3.0
FREEINTV_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FREEINTV_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(FREEINTV_MAKE_OPTS)
endef

define FREEINTV_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/freeintv_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/freeintv_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/freeintv_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
