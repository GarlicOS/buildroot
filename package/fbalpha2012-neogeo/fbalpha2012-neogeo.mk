################################################################################
#
# fbalpha2012-neogeo
#
################################################################################

FBALPHA2012_NEOGEO_VERSION = 1bdb32ba0c44a0efa8b9653a5f1856dcd8b21075
FBALPHA2012_NEOGEO_SITE = https://github.com/libretro/fbalpha2012_neogeo.git
FBALPHA2012_NEOGEO_SITE_METHOD = git
FBALPHA2012_NEOGEO_LICENSE = Non-commercial
FBALPHA2012_NEOGEO_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FBALPHA2012_NEOGEO_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(FBALPHA2012_NEOGEO_MAKE_OPTS)
endef

define FBALPHA2012_NEOGEO_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/fbalpha2012_neogeo_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/fbalpha2012_neogeo_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/fbalpha2012_neogeo_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
