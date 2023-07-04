################################################################################
#
# icofont package
#
################################################################################

ICOFONT_VERSION = 1.0
ICOFONT_SITE = package/icofont/src
ICOFONT_SITE_METHOD = local

define ICOFONT_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/icofont
	cp -f $(@D)/icofont.ttf $(TARGET_DIR)/usr/share/icofont
	mkdir -p $(TARGET_DIR)/usr/share/fonts
	ln -sf ../icofont $(TARGET_DIR)/usr/share/fonts/icofont
endef

$(eval $(generic-package))
