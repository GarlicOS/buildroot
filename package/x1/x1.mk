################################################################################
#
# x1
#
################################################################################

X1_VERSION = 4cb1e4eaab37321904144d1f1a23b2830268e8df
X1_SITE = https://github.com/libretro/xmil-libretro.git
X1_SITE_METHOD = git
X1_LICENSE = BSD
X1_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define X1_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)/libretro -f Makefile.libretro $(X1_MAKE_OPTS)
endef

define X1_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/x1_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/x1_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/libretro/x1_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
