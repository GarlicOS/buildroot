################################################################################
#
# vecx
#
################################################################################

VECX_VERSION = 8e932c1d585ae9e467186dea9e73ce38fe1490f7
VECX_SITE = https://github.com/libretro/libretro-vecx.git
VECX_SITE_METHOD = git
VECX_LICENSE = GPL-3.0
VECX_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++ \
	HAS_GPU=0

define VECX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(VECX_MAKE_OPTS)
endef

define VECX_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/vecx_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/vecx_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/vecx_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
