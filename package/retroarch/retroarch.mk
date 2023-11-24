################################################################################
#
# retroarch
#
################################################################################

RETROARCH_VERSION = 04833f54b5d7b037ac1d8f8a5b9a36fb8b6ddf0c
RETROARCH_SITE = https://github.com/libretro/RetroArch.git
RETROARCH_SITE_METHOD = git
RETROARCH_LICENSE = GPL-3.0
RETROARCH_LICENSE_FILES = COPYING
RETROARCH_DEPENDENCIES = host-pkgconf

RETROARCH_CONFIG_OPTS = \
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
	--enable-rpng

# Disable SDL2 backend by default (it is in another binary)
RETROARCH_CONFIG_OPTS += --disable-sdl2

ifeq ($(BR2_PACKAGE_ALSA_LIB),y)
RETROARCH_DEPENDENCIES += alsa-lib
RETROARCH_CONFIG_OPTS += --enable-alsa
else
RETROARCH_CONFIG_OPTS += --disable-alsa
endif

ifeq ($(BR2_PACKAGE_RETROARCH_OPENSSL),y)
RETROARCH_DEPENDENCIES += openssl
RETROARCH_CONFIG_OPTS += --enable-ssl
else
RETROARCH_CONFIG_OPTS += --disable-ssl
endif

ifeq ($(BR2_PACKAGE_PULSEAUDIO),y)
RETROARCH_DEPENDENCIES += pulseaudio
RETROARCH_CONFIG_OPTS += --enable-pulse
else
RETROARCH_CONFIG_OPTS += --disable-pulse
endif

ifeq ($(BR2_PACKAGE_TINYALSA),y)
RETROARCH_DEPENDENCIES += tinyalsa
RETROARCH_CONFIG_OPTS += --enable-tinyalsa
else
RETROARCH_CONFIG_OPTS += --disable-tinyalsa
endif

ifeq ($(BR2_PACKAGE_JACK2),y)
RETROARCH_DEPENDENCIES += jack2
RETROARCH_CONFIG_OPTS += --enable-jack
else
RETROARCH_CONFIG_OPTS += --disable-jack
endif

ifeq ($(BR2_PACKAGE_HAS_EUDEV),y)
RETROARCH_DEPENDENCIES += eudev
RETROARCH_CONFIG_OPTS += --enable-udev
else
RETROARCH_CONFIG_OPTS += --disable-udev
endif

ifeq ($(BR2_PACKAGE_HAS_UDEV),y)
RETROARCH_CONFIG_OPTS += --enable-udev
else
RETROARCH_CONFIG_OPTS += --disable-udev
endif

ifeq ($(BR2_PACKAGE_SYSTEMD),y)
RETROARCH_DEPENDENCIES += systemd
RETROARCH_CONFIG_OPTS += --enable-systemd
else
RETROARCH_CONFIG_OPTS += --disable-systemd
endif

ifeq ($(BR2_PACKAGE_MESA3D_OPENGL_GLX),y)
RETROARCH_CONFIG_OPTS += --enable-opengl
RETROARCH_DEPENDENCIES += mesa3d
else
RETROARCH_CONFIG_OPTS += --disable-opengl
endif

ifeq ($(BR2_PACKAGE_RETROARCH_EGL),y)
RETROARCH_CONFIG_OPTS += --enable-egl
RETROARCH_DEPENDENCIES += mesa3d 
else
RETROARCH_CONFIG_OPTS += --disable-egl
endif

ifeq ($(BR2_PACKAGE_RETROARCH_GLES),y)
RETROARCH_CONFIG_OPTS += --enable-opengles
RETROARCH_DEPENDENCIES += mesa3d
else
RETROARCH_CONFIG_OPTS += --disable-opengles
endif

ifeq ($(BR2_PACKAGE_ZLIB),y)
RETROARCH_DEPENDENCIES += libzlib
RETROARCH_CONFIG_OPTS += --enable-zlib
else
RETROARCH_CONFIG_OPTS += --disable-zlib
endif

ifeq ($(BR2_PACKAGE_RETROARCH_FFMPEG),y)
RETROARCH_DEPENDENCIES += ffmpeg
RETROARCH_CONFIG_OPTS += --enable-ffmpeg
else
RETROARCH_CONFIG_OPTS += --disable-ffmpeg
endif

ifeq ($(BR2_PACKAGE_RETROARCH_QT5),y)
RETROARCH_CONFIG_OPTS += --enable-qt
RETROARCH_DEPENDENCIES += qt5base
else
RETROARCH_CONFIG_OPTS += --disable-qt
endif

ifeq ($(BR2_PACKAGE_RETROARCH_KMS),y)
RETROARCH_CONFIG_OPTS += --enable-kms
RETROARCH_DEPENDENCIES += mesa3d libdrm
else
RETROARCH_CONFIG_OPTS += --disable-kms
endif

ifeq ($(BR2_PACKAGE_RETROARCH_SDL),y)
RETROARCH_CONFIG_OPTS += --enable-sdl
RETROARCH_DEPENDENCIES += sdl
else
RETROARCH_CONFIG_OPTS += --disable-sdl
endif

ifeq ($(BR2_PACKAGE_RETROARCH_WAYLAND),y)
RETROARCH_CONFIG_OPTS += --enable-wayland
RETROARCH_DEPENDENCIES += mesa3d
else
RETROARCH_CONFIG_OPTS += --disable-wayland
endif

ifeq ($(BR2_PACKAGE_RETROARCH_V4L2),y)
RETROARCH_DEPENDENCIES += libv4l
RETROARCH_CONFIG_OPTS += --enable-v4l2
else
RETROARCH_CONFIG_OPTS += --disable-v4l2
endif

ifeq ($(BR2_PACKAGE_RETROARCH_XVIDEO),y)
RETROARCH_CONFIG_OPTS += --enable-xvideo
RETROARCH_DEPENDENCIES += xlib_libXv
else
RETROARCH_CONFIG_OPTS += --disable-xvideo
endif

ifeq ($(BR2_PACKAGE_RETROARCH_RGUI_MENU),y)
RETROARCH_CONFIG_OPTS += --enable-rgui
else
RETROARCH_CONFIG_OPTS += --disable-rgui
endif

ifeq ($(BR2_PACKAGE_RETROARCH_MATERIAUI_MENU),y)
RETROARCH_CONFIG_OPTS += --enable-materialui
else
RETROARCH_CONFIG_OPTS += --disable-materialui
endif

ifeq ($(BR2_PACKAGE_RETROARCH_XMB_MENU),y)
RETROARCH_CONFIG_OPTS += --enable-xmb
else
RETROARCH_CONFIG_OPTS += --disable-xmb
endif

ifeq ($(BR2_PACKAGE_RETROARCH_OZONE_MENU),y)
RETROARCH_CONFIG_OPTS += --enable-ozone
else
RETROARCH_CONFIG_OPTS += --disable-ozone
endif

define RETROARCH_CONFIGURE_CMDS
	cd $(@D) && \
	PKG_CONF_PATH=pkg-config \
	PKG_CONFIG_PATH="$(HOST_PKG_CONFIG_PATH)" \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	LDFLAGS="-ltinyalsa" \
	./configure $(RETROARCH_CONFIG_OPTS)
endef

define RETROARCH_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D)
	$(TARGET_MAKE_ENV) compiler=$(TARGET_CC) $(MAKE) -C $(@D)/libretro-common/audio/dsp_filters
	$(TARGET_MAKE_ENV) compiler=$(TARGET_CC) $(MAKE) -C $(@D)/gfx/video_filters
endef

define RETROARCH_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/autoconfig
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install
	$(TARGET_MAKE_ENV) $(MAKE) PREFIX=$(TARGET_DIR) -C $(@D)/libretro-common/audio/dsp_filters install
	$(TARGET_MAKE_ENV) $(MAKE) PREFIX=$(TARGET_DIR) -C $(@D)/gfx/video_filters install
	$(INSTALL) -m 0644 -D $(RETROARCH_PKGDIR)/retroarch.cfg $(TARGET_DIR)/root/.config/retroarch/retroarch.cfg
	$(INSTALL) -m 0644 -D "$(RETROARCH_PKGDIR)/Internal Gamepad.cfg" "$(TARGET_DIR)/root/.config/retroarch/autoconfig/Internal Gamepad.cfg"
endef

$(eval $(generic-package))
