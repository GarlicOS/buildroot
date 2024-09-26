################################################################################
#
# melonds
#
################################################################################

MELONDS_VERSION = c6488c88cb4c7583dbcd61609e0eef441572fae8
MELONDS_SITE = https://github.com/libretro/melonds.git
MELONDS_SITE_METHOD = git
MELONDS_LICENSE = Non-commercial
MELONDS_LICENSE_FILES = LICENSE.txt
MELONDS_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++ \
	ARCH=$(BR2_NORMALIZED_ARCH)

define MELONDS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(MELONDS_MAKE_OPTS)
endef

define MELONDS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/melonds_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/melonds_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/melonds_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
