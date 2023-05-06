################################################################################
#
# ngp
#
################################################################################

NGP_VERSION = 65460e3a9ad529f6901caf669abbda11f437ab55
NGP_SITE = https://github.com/libretro/beetle-ngp-libretro.git
NGP_SITE_METHOD = git
NGP_LICENSE = GPL-2.0
NGP_LICENSE_FILES = COPYING
NGP_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define NGP_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(NGP_MAKE_OPTS)
endef

define NGP_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_ngp_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_ngp_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_ngp_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
