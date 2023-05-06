################################################################################
#
# dosbox-pure
#
################################################################################

DOSBOX_PURE_VERSION = 4fdb557e415698aae5bd90b076f76437f5e9b0b4
DOSBOX_PURE_SITE = https://github.com/libretro/dosbox-pure.git
DOSBOX_PURE_SITE_METHOD = git
DOSBOX_PURE_LICENSE = GPL-2.0
DOSBOX_PURE_LICENSE_FILES = LICENSE
DOSBOX_PURE_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define DOSBOX_PURE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(DOSBOX_PURE_MAKE_OPTS)
endef

define DOSBOX_PURE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/dosbox_pure_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/dosbox_pure_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/dosbox_pure_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
