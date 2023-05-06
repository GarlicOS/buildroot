################################################################################
#
# supergrafx
#
################################################################################

SUPERGRAFX_VERSION = 4c6f55186c17b509343f61dda9992e48299930aa
SUPERGRAFX_SITE = https://github.com/libretro/beetle-supergrafx-libretro.git
SUPERGRAFX_SITE_METHOD = git
SUPERGRAFX_LICENSE = GPL-2.0
SUPERGRAFX_LICENSE_FILES = COPYING
SUPERGRAFX_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SUPERGRAFX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(SUPERGRAFX_MAKE_OPTS)
endef

define SUPERGRAFX_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_supergrafx_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_supergrafx_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_supergrafx_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
