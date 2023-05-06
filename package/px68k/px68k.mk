################################################################################
#
# px68k
#
################################################################################

PX68K_VERSION = 4c7542e34df71e3ebab77b5867901747d58dbb38
PX68K_SITE = https://github.com/libretro/px68k-libretro.git
PX68K_SITE_METHOD = git
PX68K_LICENSE = Custom Non-Commercial
PX68K_LICENSE_FILES = COPYING
PX68K_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++ \
	platform=unix \
	HAVE_NEON=1 \
	ARCH=arm

define PX68K_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile.libretro $(PX68K_MAKE_OPTS)
endef

define PX68K_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/px68k_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/px68k_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/px68k_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
