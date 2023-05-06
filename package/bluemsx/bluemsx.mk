################################################################################
#
# bluemsx
#
################################################################################

BLUEMSX_VERSION = e21bf74bddb79ad1bbe20b4d964e7515269c669b
BLUEMSX_SITE = https://github.com/libretro/blueMSX-libretro.git
BLUEMSX_SITE_METHOD = git
BLUEMSX_LICENSE = GPL-2.0
BLUEMSX_LICENSE_FILES = Copying
BLUEMSX_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define BLUEMSX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(BLUEMSX_MAKE_OPTS)
endef

define BLUEMSX_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/bluemsx_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/bluemsx_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/bluemsx_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
