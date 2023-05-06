################################################################################
#
# quasi88
#
################################################################################

QUASI88_VERSION = 7980f2484997055a9665837bb4c325d562acb04e
QUASI88_SITE = https://github.com/libretro/quasi88-libretro.git
QUASI88_SITE_METHOD = git
QUASI88_LICENSE = BSD 3-Clause and MAME non-commercial
QUASI88_LICENSE_FILES = LICENSE
QUASI88_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define QUASI88_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(QUASI88_MAKE_OPTS)
endef

define QUASI88_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/quasi88_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/quasi88_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/quasi88_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
