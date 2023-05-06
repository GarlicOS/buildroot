################################################################################
#
# wswan
#
################################################################################

WSWAN_VERSION = 2d4edfdee1af0dca930c61200853c87cb89fe6d9
WSWAN_SITE = https://github.com/libretro/beetle-wswan-libretro
WSWAN_SITE_METHOD = git
WSWAN_LICENSE = GPL-2.0
WSWAN_LICENSE_FILES = COPYING
WSWAN_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define WSWAN_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(WSWAN_MAKE_OPTS)
endef

define WSWAN_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/mednafen_wswan_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/mednafen_wswan_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/mednafen_wswan_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
