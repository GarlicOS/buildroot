################################################################################
#
# freechaf
#
################################################################################

FREECHAF_VERSION = 4d1d5cb83b93728a63f03454e472a23055d9bbfc
FREECHAF_SITE = https://github.com/libretro/FreeChaF.git
FREECHAF_SITE_METHOD = git
FREECHAF_GIT_SUBMODULES = YES
FREECHAF_LICENSE = GPL-3.0
FREECHAF_LICENSE_FILES = LICENSE
FREECHAF_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FREECHAF_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(FREECHAF_MAKE_OPTS)
endef

define FREECHAF_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/freechaf_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/freechaf_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/freechaf_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
