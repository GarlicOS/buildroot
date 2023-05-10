################################################################################
#
# audio package
#
################################################################################

AUDIO_VERSION = 1.0
AUDIO_SITE = package/audio/src
AUDIO_SITE_METHOD = local
AUDIO_DEPENDENCIES = tinyalsa
AUDIO_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define AUDIO_BUILD_CMDS
	$(MAKE) CFLAGS="$(AUDIO_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define AUDIO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/audio $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
