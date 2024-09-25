################################################################################
#
# retroarch-assets
#
################################################################################

RETROARCH_ASSETS_VERSION = 1b138de1023272b039273d7bbc8f57573073bd25
RETROARCH_ASSETS_SITE = https://github.com/libretro/retroarch-assets.git
RETROARCH_ASSETS_SITE_METHOD = git
RETROARCH_ASSETS_DEPENDENCIES = host-pkgconf

define RETROARCH_ASSETS_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) INSTALLDIR=$(TARGET_DIR)/root/.config/retroarch/assets -C $(@D) install
endef

$(eval $(generic-package))
