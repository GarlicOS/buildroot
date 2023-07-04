################################################################################
#
# noto-sans package
#
################################################################################

NOTO_SANS_VERSION = 1.0
NOTO_SANS_SITE = package/noto-sans/src
NOTO_SANS_SITE_METHOD = local

define NOTO_SANS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSans-Regular.ttf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansEthiopic-Regular.ttf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansHebrew-Regular.ttf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansJP-Regular.ttf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansKR-Regular.otf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansSC-Regular.otf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansTC-Regular.otf $(TARGET_DIR)/usr/share/noto-sans
	cp -f $(@D)/NotoSansThai-Regular.ttf $(TARGET_DIR)/usr/share/noto-sans
	mkdir -p $(TARGET_DIR)/usr/share/fonts
	ln -sf ../noto-sans $(TARGET_DIR)/usr/share/fonts/noto-sans
endef

$(eval $(generic-package))
