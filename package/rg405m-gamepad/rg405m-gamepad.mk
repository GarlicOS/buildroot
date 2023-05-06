################################################################################
#
# rg405m-gamepad package
#
################################################################################

RG405M_GAMEPAD_VERSION = 1.0
RG405M_GAMEPAD_SITE = package/rg405m-gamepad/src
RG405M_GAMEPAD_SITE_METHOD = local
RG405M_GAMEPAD_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define RG405M_GAMEPAD_BUILD_CMDS
	$(MAKE) CFLAGS="$(RG405M_GAMEPAD_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define RG405M_GAMEPAD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rg405m-gamepad $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
