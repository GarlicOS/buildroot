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
	$(INSTALL) -D -m 0755 $(@D)/drivers/usr/lib/* $(TARGET_DIR)/usr/lib
	ln -sf rg35xx+_libmali.so $(TARGET_DIR)/usr/lib/rg35xx+_libEGL.so
	ln -sf rg35xx+_libmali.so $(TARGET_DIR)/usr/lib/rg35xx+_libGLESv1_CM.so
	ln -sf rg35xx+_libmali.so $(TARGET_DIR)/usr/lib/rg35xx+_libGLESv2.so
endef

$(eval $(generic-package))
