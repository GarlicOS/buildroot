################################################################################
#
# fake08
#
################################################################################

FAKE08_VERSION = eed6109d121e4de8ad1a2d6f81d712648462592f
FAKE08_SITE = https://github.com/jtothebell/fake-08.git
FAKE08_SITE_METHOD = git
FAKE08_GIT_SUBMODULES = YES
FAKE08_LICENSE = MIT
FAKE08_LICENSE_FILES = LICENSE.MD
FAKE08_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FAKE08_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/platform/libretro $(FAKE08_MAKE_OPTS)
endef

define FAKE08_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/fake08_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/fake08_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/platform/libretro/fake08_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
