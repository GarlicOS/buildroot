################################################################################
#
# sameduck
#
################################################################################

SAMEDUCK_VERSION = SameDuck
SAMEDUCK_SITE = https://github.com/LIJI32/SameBoy.git
SAMEDUCK_SITE_METHOD = git
SAMEDUCK_LICENSE = MIT
SAMEDUCK_LICENSE_FILES = LICENSE
SAMEDUCK_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define SAMEDUCK_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/libretro $(SAMEDUCK_MAKE_OPTS)
endef

define SAMEDUCK_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/sameduck_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/sameduck_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/build/bin/sameduck_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
