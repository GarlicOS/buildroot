################################################################################
#
# oswald package
#
################################################################################

OSWALD_VERSION = 1.0
OSWALD_SITE = package/oswald/src
OSWALD_SITE_METHOD = local

define OSWALD_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/oswald
	cp -f $(@D)/Oswald-Bold.ttf $(TARGET_DIR)/usr/share/oswald
	cp -f $(@D)/Oswald-ExtraLight.ttf $(TARGET_DIR)/usr/share/oswald
	cp -f $(@D)/Oswald-Light.ttf $(TARGET_DIR)/usr/share/oswald
	cp -f $(@D)/Oswald-Medium.ttf $(TARGET_DIR)/usr/share/oswald
	cp -f $(@D)/Oswald-Regular.ttf $(TARGET_DIR)/usr/share/oswald
	cp -f $(@D)/Oswald-SemiBold.ttf $(TARGET_DIR)/usr/share/oswald
	mkdir -p $(TARGET_DIR)/usr/share/fonts
	ln -sf ../oswald $(TARGET_DIR)/usr/share/fonts/oswald
endef

$(eval $(generic-package))
