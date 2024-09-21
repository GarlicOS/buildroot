################################################################################
#
# retroarch-glsl-shaders
#
################################################################################

RETROARCH_GLSL_SHADERS_VERSION = 5b0c15c6259849e7a6f990b5f13ec008c9168ec0
RETROARCH_GLSL_SHADERS_SITE = https://github.com/libretro/glsl-shaders.git
RETROARCH_GLSL_SHADERS_SITE_METHOD = git
RETROARCH_GLSL_SHADERS_DEPENDENCIES = host-pkgconf

define RETROARCH_GLSL_SHADERS_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) INSTALLDIR=$(TARGET_DIR)/root/.config/retroarch/shaders -C $(@D) install
endef

$(eval $(generic-package))
