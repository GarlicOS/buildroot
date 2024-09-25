################################################################################
#
# retroarch-sdl2
#
################################################################################

RETROARCH_SDL2_VERSION = c90f7aba6cb3676ecde45dea5acfbf1a652edcdd
RETROARCH_SDL2_SITE = https://github.com/libretro/RetroArch.git
RETROARCH_SDL2_SITE_METHOD = git
RETROARCH_SDL2_LICENSE = GPL-3.0
RETROARCH_SDL2_LICENSE_FILES = COPYING
RETROARCH_SDL2_DEPENDENCIES = host-pkgconf

RETROARCH_SDL2_CONFIG_OPTS = \
	--build=x86_64-linux \
	--prefix=$(TARGET_DIR)/usr \
	--sysconfdir=$(STAGING_DIR) \
	--host=$(TARGET_CC) \
	--disable-gdi \
	--disable-d3dx \
	--disable-d3d8 \
	--disable-d3d9 \
	--disable-d3d10 \
	--disable-d3d11 \
	--disable-d3d12 \
	--disable-metal \
	--disable-opengl1 \
	--disable-freetype \
	--enable-rpng

# Depend on main RetroArch binary build
RETROARCH_SDL2_DEPENDENCIES += retroarch

# Build with SDL2 backend by default
RETROARCH_SDL2_CONFIG_OPTS += --enable-sdl2
RETROARCH_SDL2_DEPENDENCIES += sdl2

ifeq ($(BR2_PACKAGE_ALSA_LIB),y)
RETROARCH_SDL2_DEPENDENCIES += alsa-lib
RETROARCH_SDL2_CONFIG_OPTS += --enable-alsa
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-alsa
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_OPENSSL),y)
RETROARCH_SDL2_DEPENDENCIES += openssl
RETROARCH_SDL2_CONFIG_OPTS += --enable-ssl
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-ssl
endif

ifeq ($(BR2_PACKAGE_PULSEAUDIO),y)
RETROARCH_SDL2_DEPENDENCIES += pulseaudio
RETROARCH_SDL2_CONFIG_OPTS += --enable-pulse
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-pulse
endif

ifeq ($(BR2_PACKAGE_TINYALSA),y)
RETROARCH_SDL2_DEPENDENCIES += tinyalsa
RETROARCH_SDL2_CONFIG_OPTS += --enable-tinyalsa
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-tinyalsa
endif

ifeq ($(BR2_PACKAGE_JACK2),y)
RETROARCH_SDL2_DEPENDENCIES += jack2
RETROARCH_SDL2_CONFIG_OPTS += --enable-jack
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-jack
endif

ifeq ($(BR2_PACKAGE_HAS_EUDEV),y)
RETROARCH_SDL2_DEPENDENCIES += eudev
RETROARCH_SDL2_CONFIG_OPTS += --enable-udev
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-udev
endif

ifeq ($(BR2_PACKAGE_HAS_UDEV),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-udev
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-udev
endif

ifeq ($(BR2_PACKAGE_SYSTEMD),y)
RETROARCH_SDL2_DEPENDENCIES += systemd
RETROARCH_SDL2_CONFIG_OPTS += --enable-systemd
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-systemd
endif

ifeq ($(BR2_PACKAGE_MESA3D_OPENGL_GLX),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-opengl
RETROARCH_SDL2_DEPENDENCIES += mesa3d
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-opengl
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_EGL),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-egl
RETROARCH_SDL2_DEPENDENCIES += mesa3d 
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-egl
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_GLES),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-opengles --enable-mali_fbdev --enable-opendingux_fbdev
RETROARCH_SDL2_DEPENDENCIES += mesa3d
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-opengles
endif

ifeq ($(BR2_PACKAGE_ZLIB),y)
RETROARCH_SDL2_DEPENDENCIES += libzlib
RETROARCH_SDL2_CONFIG_OPTS += --enable-zlib
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-zlib
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_FFMPEG),y)
RETROARCH_SDL2_DEPENDENCIES += ffmpeg
RETROARCH_SDL2_CONFIG_OPTS += --enable-ffmpeg
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-ffmpeg
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_KMS),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-kms
RETROARCH_SDL2_DEPENDENCIES += mesa3d libdrm
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-kms
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_WAYLAND),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-wayland
RETROARCH_SDL2_DEPENDENCIES += mesa3d
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-wayland
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_V4L2),y)
RETROARCH_SDL2_DEPENDENCIES += libv4l
RETROARCH_SDL2_CONFIG_OPTS += --enable-v4l2
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-v4l2
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_XVIDEO),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-xvideo
RETROARCH_SDL2_DEPENDENCIES += xlib_libXv
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-xvideo
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_RGUI_MENU),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-rgui
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-rgui
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_MATERIAUI_MENU),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-materialui
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-materialui
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_XMB_MENU),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-xmb
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-xmb
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL2_OZONE_MENU),y)
RETROARCH_SDL2_CONFIG_OPTS += --enable-ozone
else
RETROARCH_SDL2_CONFIG_OPTS += --disable-ozone
endif

define RETROARCH_SDL2_CONFIGURE_CMDS
	cd $(@D) && \
	PKG_CONF_PATH=pkg-config \
	PKG_CONFIG_PATH="$(HOST_PKG_CONFIG_PATH)" \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	LDFLAGS="-ltinyalsa" \
	./configure $(RETROARCH_SDL2_CONFIG_OPTS)
endef

define RETROARCH_SDL2_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)
endef

define RETROARCH_SDL2_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/retroarch $(TARGET_DIR)/usr/bin/retroarch-sdl2
endef

$(eval $(generic-package))
