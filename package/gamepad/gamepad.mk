################################################################################
#
# gamepad package
#
################################################################################

GAMEPAD_VERSION = 1.0
GAMEPAD_SITE = package/gamepad/src
GAMEPAD_SITE_METHOD = local
GAMEPAD_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define GAMEPAD_BUILD_CMDS
	$(MAKE) CFLAGS="$(GAMEPAD_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define GAMEPAD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/gamepad $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
