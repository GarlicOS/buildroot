################################################################################
#
# hatari
#
################################################################################

HATARI_VERSION = 1ebf0a0488580ef95c0b28f02223b31813c867c5
HATARI_SITE = https://github.com/libretro/hatari.git
HATARI_SITE_METHOD = git
HATARI_LICENSE = GPL-2.0
HATARI_LICENSE_FILES = gpl.txt
HATARI_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define HATARI_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(HATARI_MAKE_OPTS)
endef

define HATARI_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/hatari_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/hatari_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/hatari_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
