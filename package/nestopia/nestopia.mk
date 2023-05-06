################################################################################
#
# nestopia
#
################################################################################

NESTOPIA_VERSION = 66ec29fc462cd053e0aaef836868fc96f568cdfc
NESTOPIA_SITE = https://github.com/libretro/nestopia.git
NESTOPIA_SITE_METHOD = git
NESTOPIA_LICENSE = GPL-2.0
NESTOPIA_LICENSE_FILES = COPYING
NESTOPIA_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define NESTOPIA_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/libretro $(NESTOPIA_MAKE_OPTS)
endef

define NESTOPIA_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/nestopia_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/nestopia_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/libretro/nestopia_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
