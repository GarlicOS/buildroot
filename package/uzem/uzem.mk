################################################################################
#
# uzem
#
################################################################################

UZEM_VERSION = 08e39e19167727c89fb995e3fa70dde252e6aab0
UZEM_SITE = https://github.com/libretro/libretro-uzem.git
UZEM_SITE_METHOD = git
UZEM_LICENSE = MIT
UZEM_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define UZEM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(UZEM_MAKE_OPTS)
endef

define UZEM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/uzem_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/uzem_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/uzem_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
