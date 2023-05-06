################################################################################
#
# crocods
#
################################################################################

CROCODS_VERSION = f1b4b91291ba1e8e7c0be02269cd0d75c7fa71b9
CROCODS_SITE = https://github.com/libretro/libretro-crocods.git
CROCODS_SITE_METHOD = git
CROCODS_LICENSE = MIT
CROCODS_LICENSE_FILES = LICENSE
CROCODS_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define CROCODS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(CROCODS_MAKE_OPTS)
endef

define CROCODS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/crocods_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/crocods_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/crocods_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
