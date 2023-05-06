################################################################################
#
# sprdaudio package
#
################################################################################

SPRDAUDIO_VERSION = 1.0
SPRDAUDIO_SITE = package/sprdaudio/src
SPRDAUDIO_SITE_METHOD = local
SPRDAUDIO_DEPENDENCIES = tinyalsa
SPRDAUDIO_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define SPRDAUDIO_BUILD_CMDS
	$(MAKE) CFLAGS="$(SPRDAUDIO_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define SPRDAUDIO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/sprdaudio $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
