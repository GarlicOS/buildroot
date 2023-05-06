################################################################################
#
# prosystem
#
################################################################################

PROSYSTEM_VERSION = 763ad22c7de51c8f06d6be0d49c554ce6a94a29b
PROSYSTEM_SITE = https://github.com/libretro/prosystem-libretro.git
PROSYSTEM_SITE_METHOD = git
PROSYSTEM_LICENSE = GPL-2.0
PROSYSTEM_LICENSE_FILES = License.txt
PROSYSTEM_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define PROSYSTEM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(PROSYSTEM_MAKE_OPTS)
endef

define PROSYSTEM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/prosystem_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/prosystem_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/prosystem_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
