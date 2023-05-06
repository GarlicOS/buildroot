################################################################################
#
# mame2003-xtreme
#
################################################################################

MAME2003_XTREME_VERSION = 115a0d8cd6675ac9422cd90f54d07d38f43c9e70
MAME2003_XTREME_SITE = https://github.com/KMFDManic/mame2003-xtreme.git
MAME2003_XTREME_SITE_METHOD = git
MAME2003_XTREME_LICENSE = MAME
MAME2003_XTREME_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define MAME2003_XTREME_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(MAME2003_XTREME_MAKE_OPTS)
endef

define MAME2003_XTREME_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	$(INSTALL) -m 0644 -D $(@D)/info/km_mame2003_xtreme_amped_libretro.info $(TARGET_DIR)/root/.config/retroarch/cores/
	$(INSTALL) -m 0755 -D $(@D)/km_mame2003_xtreme_amped_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
