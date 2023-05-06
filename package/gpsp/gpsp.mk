################################################################################
#
# gpsp
#
################################################################################

GPSP_VERSION = f4e65d15c04797b2473eb6492d51465fe98612d8
GPSP_SITE = https://github.com/libretro/gpsp.git
GPSP_SITE_METHOD = git
GPSP_LICENSE = GPL-2.0
GPSP_LICENSE_FILES = COPYING
GPSP_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

ifeq ($(ARCH),arm)
	GPSP_MAKE_OPTS += \
		platform=armv8
else ifeq ($(ARCH),aarch64)
	GPSP_MAKE_OPTS += \
		platform=arm64
endif

define GPSP_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(GPSP_MAKE_OPTS)
endef

define GPSP_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/gpsp_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/gpsp_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/gpsp_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
