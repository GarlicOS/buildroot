################################################################################
#
# potator
#
################################################################################

POTATOR_VERSION = 06ec3b724b6fccda6805e3baa76c00c3bc024f23
POTATOR_SITE = https://github.com/libretro/potator.git
POTATOR_SITE_METHOD = git
POTATOR_LICENSE = Public Domain
POTATOR_LICENSE_FILES = LICENSE
POTATOR_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define POTATOR_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/platform/libretro $(POTATOR_MAKE_OPTS)
endef

define POTATOR_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/potator_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/potator_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/platform/libretro/potator_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
