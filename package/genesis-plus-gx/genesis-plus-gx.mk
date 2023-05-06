################################################################################
#
# genesis-plus-gx
#
################################################################################

GENESIS_PLUS_GX_VERSION = 790776699bea2a570535e1632481aa27174c8dce
GENESIS_PLUS_GX_SITE = https://github.com/libretro/Genesis-Plus-GX.git
GENESIS_PLUS_GX_SITE_METHOD = git
GENESIS_PLUS_GX_LICENSE = Non-commercial
GENESIS_PLUS_GX_LICENSE_FILES = LICENSE.txt
GENESIS_PLUS_GX_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define GENESIS_PLUS_GX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(GENESIS_PLUS_GX_MAKE_OPTS)
endef

define GENESIS_PLUS_GX_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/genesis_plus_gx_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/genesis_plus_gx_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/genesis_plus_gx_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
