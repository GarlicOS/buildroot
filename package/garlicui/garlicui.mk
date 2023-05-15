################################################################################
#
# garlicui package
#
################################################################################

GARLICUI_VERSION = 1.0
GARLICUI_SITE = package/garlicui/src
GARLICUI_SITE_METHOD = local
GARLICUI_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define GARLICUI_BUILD_CMDS
	$(MAKE) CFLAGS="$(GARLICUI_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define GARLICUI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/garlicui $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
