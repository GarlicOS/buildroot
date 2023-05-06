################################################################################
#
# o2em
#
################################################################################

O2EM_VERSION = a2a12472fde910b6089ac3ca6de805bd58a9c999
O2EM_SITE = https://github.com/libretro/libretro-o2em.git
O2EM_SITE_METHOD = git
O2EM_LICENSE = Artistic License
O2EM_LICENSE_FILES = COPYING
O2EM_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++

define O2EM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) $(O2EM_MAKE_OPTS)
endef

define O2EM_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/o2em_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/o2em_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/o2em_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
