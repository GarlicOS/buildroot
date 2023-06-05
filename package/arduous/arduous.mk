################################################################################
#
# arduous
#
################################################################################

ARDUOUS_VERSION = aed50506962df6f965748e888b3fe7027ddb410d
ARDUOUS_SITE = https://github.com/libretro/arduous.git
ARDUOUS_SITE_METHOD = git
ARDUOUS_GIT_SUBMODULES = YES
ARDUOUS_LICENSE = GPLv3
ARDUOUS_LICENSE_FILES = COPYING

ARDUOUS_CONF_OPTS = \
	-DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
	-DCMAKE_BUILD_TYPE=Release

define ARDUOUS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/arduous_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/arduous_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/arduous_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(cmake-package))
