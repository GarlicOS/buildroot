################################################################################
#
# scummvm
#
################################################################################

SCUMMVM_VERSION = bb96f82002eb6105434e7a2285ccdd71dee9053e
SCUMMVM_SITE = https://github.com/libretro/scummvm.git
SCUMMVM_SITE_METHOD = git
SCUMMVM_LICENSE = GPL-3.0
SCUMMVM_LICENSE_FILES = COPYING
SCUMMVM_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++ \
	PKG_CONFIG_LIBDIR=$(STAGING_DIR)/usr/lib/pkgconfig \
	PATH=$(STAGING_DIR)/usr/bin:$(PATH)

#define SCUMMVM_CONFIGURE_CMDS
#	cd $(@D) && $(SCUMMVM_MAKE_OPTS) ./configure --host=$(ARCH)-linux --backend=libretro
#endef

define SCUMMVM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/backends/platform/libretro $(SCUMMVM_MAKE_OPTS)
endef

define SCUMMVM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/scummvm_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/scummvm_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/backends/platform/libretro/scummvm_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
