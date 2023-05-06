################################################################################
#
# pce-fast
#
################################################################################

PCE_FAST_VERSION = a53a9df3c76323c015d95d9a6bc4e09dec5266d2
PCE_FAST_SITE = https://github.com/libretro/beetle-pce-fast-libretro.git
PCE_FAST_SITE_METHOD = git
PCE_FAST_LICENSE = GPL-2.0
PCE_FAST_LICENSE_FILES = COPYING
PCE_FAST_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define PCE_FAST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(PCE_FAST_MAKE_OPTS)
endef

define PCE_FAST_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_pce_fast_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_pce_fast_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_pce_fast_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
