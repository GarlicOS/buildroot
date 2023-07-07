################################################################################
#
# garlicui package
#
################################################################################

GARLICUI_VERSION = 2.0
GARLICUI_SITE = package/garlicui/src
GARLICUI_SITE_METHOD = local
GARLICUI_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/librsvg-2.0 -I$(STAGING_DIR)/usr/include/glib-2.0 -I$(STAGING_DIR)/usr/lib/glib-2.0/include -I$(STAGING_DIR)/usr/include/cairo -I$(STAGING_DIR)/usr/include/gdk-pixbuf-2.0 -I$(STAGING_DIR)/usr/include/libxml2 -I$(STAGING_DIR)/usr/include/cjson
GARLICUI_DEPENDENCIES = librsvg sdl sdl_gfx sdl_image sdl_ttf libxml2 cjson icu

define GARLICUI_BUILD_CMDS
	$(MAKE) CFLAGS="$(GARLICUI_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" VERSION=$(GARLICUI_VERSION) -C $(@D)
endef

define GARLICUI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/garlicui $(TARGET_DIR)/usr/bin
	mkdir -p $(TARGET_DIR)/usr/share
	cp -rf $(@D)/icons $(TARGET_DIR)/usr/share
	cp -rf $(@D)/locale $(TARGET_DIR)/usr/share
	cp -rf $(@D)/library $(TARGET_DIR)/usr/share
	rm -f $(TARGET_DIR)/usr/share/locale/*/LC_MESSAGES/garlicui.po
endef

$(eval $(generic-package))
