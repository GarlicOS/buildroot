################################################################################
#
# display package
#
################################################################################

DISPLAY_VERSION = 1.0
DISPLAY_SITE = package/display/src
DISPLAY_SITE_METHOD = local
DISPLAY_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define DISPLAY_BUILD_CMDS
	$(MAKE) CFLAGS="$(DISPLAY_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define DISPLAY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/display $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
