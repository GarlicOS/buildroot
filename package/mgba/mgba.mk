################################################################################
#
# mgba
#
################################################################################

MGBA_VERSION = a69c3434afe8b26cb8f9463077794edfa7d5efad
MGBA_SITE = https://github.com/libretro/mgba.git
MGBA_SITE_METHOD = git
MGBA_LICENSE = MPL-2.0
MGBA_LICENSE_FILES = LICENSE
MGBA_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define MGBA_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(MGBA_MAKE_OPTS)
endef

define MGBA_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mgba_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mgba_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mgba_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
