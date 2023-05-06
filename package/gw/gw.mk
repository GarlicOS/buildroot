################################################################################
#
# gw
#
################################################################################

GW_VERSION = 19a1cb3105ca4a82139fb4994e7995fd956f6f8d
GW_SITE = https://github.com/libretro/gw-libretro.git
GW_SITE_METHOD = git
GW_LICENSE = Zlib
GW_LICENSE_FILES = LICENSE
GW_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define GW_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(GW_MAKE_OPTS)
endef

define GW_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/gw_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/gw_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/gw_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
