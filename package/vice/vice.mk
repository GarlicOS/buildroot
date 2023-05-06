################################################################################
#
# vice
#
################################################################################

VICE_VERSION = 0dfc133159c18cc4bd3c2bf3ce0adbb5895d85d7
VICE_SITE = https://github.com/libretro/vice-libretro.git
VICE_SITE_METHOD = git
VICE_LICENSE = GPL-2.0
VICE_LICENSE_FILES = COPYING
VICE_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define VICE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) EMUTYPE=x64 $(VICE_MAKE_OPTS)
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) EMUTYPE=x128 $(VICE_MAKE_OPTS)
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) EMUTYPE=xvic $(VICE_MAKE_OPTS)
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) EMUTYPE=xpet $(VICE_MAKE_OPTS)
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) EMUTYPE=xplus4 $(VICE_MAKE_OPTS)
endef

define VICE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vice_x64_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vice_x64_libretro.info
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vice_x128_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vice_x128_libretro.info
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vice_xvic_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vice_xvic_libretro.info
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vice_xpet_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vice_xpet_libretro.info
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vice_xplus4_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vice_xplus4_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/vice_x64_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
	$(INSTALL) -m 0755 -D $(@D)/vice_x128_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
	$(INSTALL) -m 0755 -D $(@D)/vice_xvic_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
	$(INSTALL) -m 0755 -D $(@D)/vice_xpet_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
	$(INSTALL) -m 0755 -D $(@D)/vice_xplus4_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
