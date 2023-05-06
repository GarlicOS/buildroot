################################################################################
#
# fbneo
#
################################################################################

FBNEO_VERSION = 004943bf7e2ef6b367b04b53641434149fc1f86c
FBNEO_SITE = https://github.com/libretro/FBNeo.git
FBNEO_SITE_METHOD = git
FBNEO_LICENSE = Non-commercial
FBNEO_LICENSE_FILES = LICENSE.md
FBNEO_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FBNEO_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/src/burner/libretro generate-files $(FBNEO_MAKE_OPTS)
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/src/burner/libretro $(FBNEO_MAKE_OPTS)
endef

define FBNEO_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/fbneo_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/fbneo_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/src/burner/libretro/fbneo_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
