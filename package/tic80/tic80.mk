################################################################################
#
# tic80
#
################################################################################

TIC80_VERSION = 067d90d2e2380540398b0867582f327dbea13832
TIC80_SITE = https://github.com/nesbox/TIC-80.git
TIC80_SITE_METHOD = git
TIC80_GIT_SUBMODULES = YES
TIC80_LICENSE = MIT
TIC80_LICENSE_FILES = LICENSE

TIC80_CONF_OPTS = \
	-DBUILD_PLAYER=OFF \
	-DBUILD_SOKOL=OFF \
	-DBUILD_SDL=OFF \
	-DBUILD_DEMO_CARTS=OFF \
	-DBUILD_LIBRETRO=ON \
	-DBUILD_WITH_MRUBY=OFF \
	-DCMAKE_BUILD_TYPE=Release

define TIC80_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/tic80_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/tic80_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/lib/libjanet.so $(TARGET_DIR)/usr/lib/
	$(INSTALL) -m 0755 -D $(@D)/lib/libuv.so.1.0.0 $(TARGET_DIR)/usr/lib/
	$(INSTALL) -m 0755 -D $(@D)/lib/libzip.so $(TARGET_DIR)/usr/lib/
	ln -s libuv.so.1.0.0 $(TARGET_DIR)/usr/lib/libuv.so.1
	ln -s libuv.so.1 $(TARGET_DIR)/usr/lib/libuv.so
	$(INSTALL) -m 0755 -D $(@D)/lib/tic80_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(cmake-package))
