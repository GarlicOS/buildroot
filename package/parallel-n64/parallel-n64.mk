################################################################################
#
# parallel-n64
#
################################################################################

PARALLEL_N64_VERSION = 1b57f9199b1f8a4510f7f89f14afa9cabf9b3bdd
PARALLEL_N64_SITE = https://github.com/libretro/parallel-n64.git
PARALLEL_N64_SITE_METHOD = git
PARALLEL_N64_DEPENDENCIES = mesa3d
PARALLEL_N64_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CC=$(TARGET_CROSS)gcc \
	CXX=$(TARGET_CROSS)g++ \
	FORCE_GLES=1 \
	"COREFLAGS=-D__LIBRETRO__ -DM64P_PLUGIN_API -DM64P_CORE_PROTOTYPES -D_ENDUSER_RELEASE -DSINC_LOWER_QUALITY -I$(STAGING_DIR)/usr/include"

ifeq ($(BR2_aarch64),y)
PARALLEL_N64_MAKE_OPTS += CPUFLAGS=-DARM_FIX=1 WITH_DYNAREC=aarch64
else ifeq ($(BR2_arm),y)
PARALLEL_N64_MAKE_OPTS += CPUFLAGS=-DARM_FIX=1 WITH_DYNAREC=arm HAVE_NEON=1
endif

define PARALLEL_N64_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) -f Makefile $(PARALLEL_N64_MAKE_OPTS)
endef

define PARALLEL_N64_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/root/.config/retroarch/cores
	wget -O $(TARGET_DIR)/root/.config/retroarch/cores/parallel_n64_libretro.info https://raw.githubusercontent.com/libretro/libretro-super/master/dist/info/parallel_n64_libretro.info
	$(INSTALL) -m 0755 -D $(@D)/parallel_n64_libretro.so $(TARGET_DIR)/root/.config/retroarch/cores/
endef

$(eval $(generic-package))
