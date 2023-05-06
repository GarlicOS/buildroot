################################################################################
#
# fuse
#
################################################################################

FUSE_VERSION = 3f9344ddf88dfd251d95cffdea615692479e8cc6
FUSE_SITE = https://github.com/libretro/fuse-libretro.git
FUSE_SITE_METHOD = git
FUSE_LICENSE = GPL-3.0
FUSE_LICENSE_FILES = LICENSE
FUSE_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FUSE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(FUSE_MAKE_OPTS)
endef

define FUSE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/fuse_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/fuse_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/fuse_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
