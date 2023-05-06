################################################################################
#
# vb
#
################################################################################

VB_VERSION = dd6393f76ff781df0f4e8c953f5b053b1e61b313
VB_SITE = https://github.com/libretro/beetle-vb-libretro.git
VB_SITE_METHOD = git
VB_LICENSE = GPL-2.0
VB_LICENSE_FILES = COPYING
VB_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define VB_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(VB_MAKE_OPTS)
endef

define VB_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_vb_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_vb_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_vb_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
