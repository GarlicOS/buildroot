################################################################################
#
# pcfx
#
################################################################################

PCFX_VERSION = 51fc3b75641533ced80c346728c0196a59d79e69
PCFX_SITE = https://github.com/libretro/beetle-pcfx-libretro.git
PCFX_SITE_METHOD = git
PCFX_LICENSE = GPL-2.0
PCFX_LICENSE_FILES = COPYING
PCFX_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++ \
	IS_X86=0

define PCFX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(PCFX_MAKE_OPTS)
endef

define PCFX_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_pcfx_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_pcfx_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_pcfx_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
