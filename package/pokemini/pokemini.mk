################################################################################
#
# pokemini
#
################################################################################

POKEMINI_VERSION = 9ba2c2d98bef98794095f3ef50e22f1a3cbc6166
POKEMINI_SITE = https://github.com/libretro/PokeMini.git
POKEMINI_SITE_METHOD = git
POKEMINI_LICENSE = GPL-3.0
POKEMINI_LICENSE_FILES = LICENSE
POKEMINI_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define POKEMINI_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(POKEMINI_MAKE_OPTS)
endef

define POKEMINI_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/pokemini_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/pokemini_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/pokemini_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
