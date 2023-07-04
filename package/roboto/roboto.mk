################################################################################
#
# roboto package
#
################################################################################

ROBOTO_VERSION = 1.0
ROBOTO_SITE = package/roboto/src
ROBOTO_SITE_METHOD = local

define ROBOTO_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Black.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-BlackItalic.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Bold.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-BoldItalic.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Italic.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Light.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-LightItalic.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Medium.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-MediumItalic.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Regular.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-Thin.ttf $(TARGET_DIR)/usr/share/roboto
	cp -f $(@D)/src/Roboto-ThinItalic.ttf $(TARGET_DIR)/usr/share/roboto
	mkdir -p $(TARGET_DIR)/usr/share/fonts
	ln -sf ../roboto $(TARGET_DIR)/usr/share/fonts/roboto
endef

$(eval $(generic-package))
