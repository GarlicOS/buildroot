################################################################################
#
# atari800
#
################################################################################

ATARI800_VERSION = 86be15db82e8c4275a7854d8b61839e87f06ef30
ATARI800_SITE = https://github.com/libretro/libretro-atari800.git
ATARI800_SITE_METHOD = git
ATARI800_LICENSE = GPL-2.0
ATARI800_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define ATARI800_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(ATARI800_MAKE_OPTS)
endef

define ATARI800_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/atari800_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/atari800_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/atari800_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
