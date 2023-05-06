################################################################################
#
# supafaust
#
################################################################################

SUPAFAUST_VERSION = 75c658cce454e58ae04ea252f53a31c60d61548e
SUPAFAUST_SITE = https://github.com/libretro/supafaust.git
SUPAFAUST_SITE_METHOD = git
SUPAFAUST_LICENSE = GPL-2.0
SUPAFAUST_LICENSE_FILES = COPYING
SUPAFAUST_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SUPAFAUST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(SUPAFAUST_MAKE_OPTS)
endef

define SUPAFAUST_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_supafaust_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_supafaust_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_supafaust_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
