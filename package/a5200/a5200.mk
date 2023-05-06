################################################################################
#
# a5200
#
################################################################################

A5200_VERSION = 44455983830bb19513bf531388ba8f7cde9e50ab
A5200_SITE = https://github.com/libretro/a5200.git
A5200_SITE_METHOD = git
A5200_LICENSE = GPL-2.0
A5200_LICENSE_FILES = License.txt
A5200_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define A5200_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(A5200_MAKE_OPTS)
endef

define A5200_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/a5200_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/a5200_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/a5200_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
