################################################################################
#
# picodrive
#
################################################################################

PICODRIVE_VERSION = ff023d436f794f5e9d3cf53865af96b5eb90ea9d
PICODRIVE_SITE = https://github.com/libretro/picodrive.git
PICODRIVE_SITE_METHOD = git
PICODRIVE_GIT_SUBMODULES = YES
PICODRIVE_LICENSE = MAME
PICODRIVE_LICENSE_FILES = COPYING
PICODRIVE_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define PICODRIVE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(PICODRIVE_MAKE_OPTS)
endef

define PICODRIVE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/picodrive_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/picodrive_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/picodrive_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
