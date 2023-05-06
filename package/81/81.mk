################################################################################
#
# 81
#
################################################################################

81_VERSION = 340a51b250fb8fbf1a9e5d3ad3924044250064e0
81_SITE = https://github.com/libretro/81-libretro.git
81_SITE_METHOD = git
81_LICENSE = GPL-3.0
81_LICENSE_FILES = LICENSE
81_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define 81_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(81_MAKE_OPTS)
endef

define 81_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/81_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/81_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/81_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
