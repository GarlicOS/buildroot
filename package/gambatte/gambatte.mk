################################################################################
#
# gambatte
#
################################################################################

GAMBATTE_VERSION = 9574b6ef5961dcadc3fb5601779b673147515073
GAMBATTE_SITE = https://github.com/libretro/gambatte-libretro.git
GAMBATTE_SITE_METHOD = git
GAMBATTE_LICENSE = GPL-2.0
GAMBATTE_LICENSE_FILES = COPYING
GAMBATTE_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define GAMBATTE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(GAMBATTE_MAKE_OPTS)
endef

define GAMBATTE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/gambatte_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/gambatte_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/gambatte_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
