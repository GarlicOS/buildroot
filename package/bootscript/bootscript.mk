################################################################################
#
# bootscript package
#
################################################################################

BOOTSCRIPT_VERSION = 1.0
BOOTSCRIPT_SITE = package/bootscript/src
BOOTSCRIPT_SITE_METHOD = local

define BOOTSCRIPT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/init $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
