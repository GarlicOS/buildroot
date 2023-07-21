#ifndef SUNXI_H
#define SUNXI_H

#include <stdbool.h>

typedef enum
{
	DISP_OUTPUT_TYPE_NONE = 0,
	DISP_OUTPUT_TYPE_LCD  = 1,
	DISP_OUTPUT_TYPE_TV   = 2,
	DISP_OUTPUT_TYPE_HDMI = 4,
	DISP_OUTPUT_TYPE_VGA  = 8
} disp_output_type;

typedef enum
{
	DISP_TV_MOD_480I             = 0,
	DISP_TV_MOD_576I             = 1,
	DISP_TV_MOD_480P             = 2,
	DISP_TV_MOD_576P             = 3,
	DISP_TV_MOD_720P_50HZ        = 4,
	DISP_TV_MOD_720P_60HZ        = 5,
	DISP_TV_MOD_1080I_50HZ       = 6,
	DISP_TV_MOD_1080I_60HZ       = 7,
	DISP_TV_MOD_1080P_24HZ       = 8,
	DISP_TV_MOD_1080P_50HZ       = 9,
	DISP_TV_MOD_1080P_60HZ       = 0xa,
	DISP_TV_MOD_1080P_24HZ_3D_FP = 0x17,
	DISP_TV_MOD_720P_50HZ_3D_FP  = 0x18,
	DISP_TV_MOD_720P_60HZ_3D_FP  = 0x19,
	DISP_TV_MOD_1080P_25HZ       = 0x1a,
	DISP_TV_MOD_1080P_30HZ       = 0x1b,
	DISP_TV_MOD_PAL              = 0xb,
	DISP_TV_MOD_PAL_SVIDEO       = 0xc,
	DISP_TV_MOD_NTSC             = 0xe,
	DISP_TV_MOD_NTSC_SVIDEO      = 0xf,
	DISP_TV_MOD_PAL_M            = 0x11,
	DISP_TV_MOD_PAL_M_SVIDEO     = 0x12,
	DISP_TV_MOD_PAL_NC           = 0x14,
	DISP_TV_MOD_PAL_NC_SVIDEO    = 0x15,
	DISP_TV_MOD_3840_2160P_30HZ  = 0x1c,
	DISP_TV_MOD_3840_2160P_25HZ  = 0x1d,
	DISP_TV_MOD_3840_2160P_24HZ  = 0x1e,
	DISP_TV_MODE_NUM             = 0x1f
} disp_tv_mode;

int switch_sunxi_device(disp_output_type output_type, disp_tv_mode tv_mode);
int toggle_vsync(bool enabled);

#endif