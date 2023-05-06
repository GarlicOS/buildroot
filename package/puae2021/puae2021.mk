################################################################################
#
# puae2021
#
################################################################################

PUAE2021_VERSION = 2.6.1
PUAE2021_SITE = https://github.com/libretro/libretro-uae.git
PUAE2021_SITE_METHOD = git
PUAE2021_LICENSE = GPL-2.0
PUAE2021_LICENSE_FILES = COPYING
PUAE2021_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define PUAE2021_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(PUAE2021_MAKE_OPTS)
endef

define PUAE2021_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/puae2021_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/puae2021_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/puae2021_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
