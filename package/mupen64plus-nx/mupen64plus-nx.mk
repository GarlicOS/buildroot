################################################################################
#
# pcsx-rearmed
#
################################################################################

MUPEN64PLUS_NX_VERSION = 26fd1edd640ff3db49dd5ebb7e54f0de6600fc45
MUPEN64PLUS_NX_SITE = https://github.com/libretro/mupen64plus-libretro-nx.git
MUPEN64PLUS_NX_SITE_METHOD = git
MUPEN64PLUS_NX_LICENSE = GPL-2.0
MUPEN64PLUS_NX_LICENSE_FILES = COPYING
MUPEN64PLUS_NX_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_NX_MAKE_OPTS += WITH_DYNAREC=aarch64 FORCE_GLES3=1
else ifeq ($(BR2_arm),y)
MUPEN64PLUS_NX_MAKE_OPTS += WITH_DYNAREC=arm HAVE_NEON=1 HAVE_NEON_ASM=1 FORCE_GLES3=1
else
MUPEN64PLUS_NX_MAKE_OPTS += FORCE_GLES3=1
endif

define MUPEN64PLUS_NX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile $(MUPEN64PLUS_NX_MAKE_OPTS)
endef

define MUPEN64PLUS_NX_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mupen64plus_next_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mupen64plus_next_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mupen64plus_next_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
