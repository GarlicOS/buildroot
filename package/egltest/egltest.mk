################################################################################
#
# egltest package
#
################################################################################

EGLTEST_VERSION = 1.0.0
EGLTEST_SITE = package/egltest/src
EGLTEST_SITE_METHOD = local
EGLTEST_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include

define EGLTEST_BUILD_CMDS
	$(MAKE) CFLAGS="$(EGLTEST_CFLAGS)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" VERSION=$(EGLTEST_VERSION) -C $(@D)
endef

define EGLTEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/egltest $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
