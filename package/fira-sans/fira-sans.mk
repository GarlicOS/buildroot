################################################################################
#
# fira-sans package
#
################################################################################

FIRA_SANS_VERSION = 1.0
FIRA_SANS_SITE = package/fira-sans/src
FIRA_SANS_SITE_METHOD = local

define FIRA_SANS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Black.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-BlackItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Bold.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-BoldItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-ExtraBold.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-ExtraBoldItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-ExtraLight.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-ExtraLightItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Italic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Light.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-LightItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Medium.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-MediumItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Regular.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-SemiBold.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-SemiBoldItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-Thin.ttf $(TARGET_DIR)/usr/share/fira-sans
	cp -f $(@D)/FiraSans-ThinItalic.ttf $(TARGET_DIR)/usr/share/fira-sans
	mkdir -p $(TARGET_DIR)/usr/share/fonts
	ln -sf ../fira-sans $(TARGET_DIR)/usr/share/fonts/fira-sans
endef

$(eval $(generic-package))
