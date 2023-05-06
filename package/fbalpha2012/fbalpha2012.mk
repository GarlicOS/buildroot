################################################################################
#
# fbalpha2012
#
################################################################################

FBALPHA2012_VERSION = 7f8860543a81ba79c0e1ce1aa219af44568c628a
FBALPHA2012_SITE = https://github.com/libretro/fbalpha2012.git
FBALPHA2012_SITE_METHOD = git
FBALPHA2012_LICENSE = Non-commercial
FBALPHA2012_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define FBALPHA2012_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/svn-current/trunk -f makefile.libretro $(FBALPHA2012_MAKE_OPTS)
endef

define FBALPHA2012_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/fbalpha2012_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/fbalpha2012_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/svn-current/trunk/fbalpha2012_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
