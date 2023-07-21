#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sunxi.h"

#define DISPLAY_ENGINE_PATH "/dev/disp"

typedef enum
{
	DISP_RESERVE0                     = 0x00,
	DISP_RESERVE1                     = 0x01,
	DISP_SET_BKCOLOR                  = 0x03,
	DISP_GET_BKCOLOR                  = 0x04,
	DISP_SET_COLORKEY                 = 0x05,
	DISP_GET_COLORKEY                 = 0x06,
	DISP_GET_SCN_WIDTH                = 0x07,
	DISP_GET_SCN_HEIGHT               = 0x08,
	DISP_GET_OUTPUT_TYPE              = 0x09,
	DISP_SET_EXIT_MODE                = 0x0A,
	DISP_VSYNC_EVENT_EN               = 0x0B,
	DISP_BLANK                        = 0x0C,
	DISP_SHADOW_PROTECT               = 0x0D,
	DISP_HWC_COMMIT                   = 0x0E,
	DISP_DEVICE_SWITCH                = 0x0F,
	DISP_GET_OUTPUT                   = 0x10,
	DISP_SET_COLOR_RANGE              = 0x11,
	DISP_GET_COLOR_RANGE              = 0x12,
	DISP_LAYER_ENABLE                 = 0x40,
	DISP_LAYER_DISABLE                = 0x41,
	DISP_LAYER_SET_INFO               = 0x42,
	DISP_LAYER_GET_INFO               = 0x43,
	DISP_LAYER_TOP                    = 0x44,
	DISP_LAYER_BOTTOM                 = 0x45,
	DISP_LAYER_GET_FRAME_ID           = 0x46,
	DISP_LAYER_SET_CONFIG             = 0x47,
	DISP_LAYER_GET_CONFIG             = 0x48,
	DISP_LAYER_SET_CONFIG2            = 0x49,
	DISP_HDMI_SUPPORT_MODE            = 0xc4,
	DISP_SET_TV_HPD                   = 0xc5,
	DISP_HDMI_GET_EDID                = 0xc6,
	DISP_LCD_ENABLE                   = 0x100,
	DISP_LCD_DISABLE                  = 0x101,
	DISP_LCD_SET_BRIGHTNESS           = 0x102,
	DISP_LCD_GET_BRIGHTNESS           = 0x103,
	DISP_LCD_BACKLIGHT_ENABLE         = 0x104,
	DISP_LCD_BACKLIGHT_DISABLE        = 0x105,
	DISP_LCD_SET_SRC                  = 0x106,
	DISP_LCD_SET_FPS                  = 0x107,
	DISP_LCD_GET_FPS                  = 0x108,
	DISP_LCD_GET_SIZE                 = 0x109,
	DISP_LCD_GET_MODEL_NAME           = 0x10a,
	DISP_LCD_SET_GAMMA_TABLE          = 0x10b,
	DISP_LCD_GAMMA_CORRECTION_ENABLE  = 0x10c,
	DISP_LCD_GAMMA_CORRECTION_DISABLE = 0x10d,
	DISP_LCD_USER_DEFINED_FUNC        = 0x10e,
	DISP_LCD_CHECK_OPEN_FINISH        = 0x10f,
	DISP_LCD_CHECK_CLOSE_FINISH       = 0x110,
	DISP_CAPTURE_START                = 0x140,
	DISP_CAPTURE_STOP                 = 0x141,
	DISP_CAPTURE_COMMIT               = 0x142,
	DISP_ENHANCE_ENABLE               = 0x180,
	DISP_ENHANCE_DISABLE              = 0x181,
	DISP_ENHANCE_GET_EN               = 0x182,
	DISP_ENHANCE_SET_WINDOW           = 0x183,
	DISP_ENHANCE_GET_WINDOW           = 0x184,
	DISP_ENHANCE_SET_MODE             = 0x185,
	DISP_ENHANCE_GET_MODE             = 0x186,
	DISP_ENHANCE_DEMO_ENABLE          = 0x187,
	DISP_ENHANCE_DEMO_DISABLE         = 0x188,
	DISP_SMBL_ENABLE                  = 0x200,
	DISP_SMBL_DISABLE                 = 0x201,
	DISP_SMBL_GET_EN                  = 0x202,
	DISP_SMBL_SET_WINDOW              = 0x203,
	DISP_SMBL_GET_WINDOW              = 0x204,
	DISP_FB_REQUEST                   = 0x280,
	DISP_FB_RELEASE                   = 0x281,
	DISP_MEM_REQUEST                  = 0x2c0,
	DISP_MEM_RELEASE                  = 0x2c1,
	DISP_MEM_GETADR                   = 0x2c2,
	DISP_print                        = 0xffff
} disp_command;

int switch_sunxi_device(disp_output_type output_type, disp_tv_mode tv_mode)
{
	// The result
	int result = -1;

	// Open the display engine
	int disp = open(DISPLAY_ENGINE_PATH, O_RDWR);

	// We managed to open the display engine
	if (disp >= 0)
	{
		// The argument list
		unsigned long arg[3];

		// Initialize the argument list
		memset(arg, 0, sizeof(arg));

		// Set the display channel
		// arg[0] = 0;

		// Set the output type
		arg[1] = (unsigned long)output_type;

		// Set the TV mode (ignored on DISP_OUTPUT_TYPE_LCD & DISP_OUTPUT_TYPE_NONE)
		arg[2] = (unsigned long)tv_mode;

		// Looping for resiliency
		while (result != 0)
		{
			// Switch the display device
			result = ioctl(disp, DISP_DEVICE_SWITCH, (void*)arg);
		}

		// Close the display engine
		close(disp);
	}

	// Return the result
	return result;
}

int toggle_vsync(bool enabled)
{
	// The result
	int result = -1;

	// Open the display engine
	int disp = open(DISPLAY_ENGINE_PATH, O_RDWR);

	// We managed to open the display engine
	if (disp >= 0)
	{
		// The argument list
		unsigned long arg[3];

		// Initialize the argument list
		memset(arg, 0, sizeof(arg));

		// Set the display channel
		// arg[0] = 0;

		// Set the vsync enabled state
		arg[1] = enabled ? 1 : 0;

		// Looping for resiliency
		while (result != 0)
		{
			// Toggle the vsync enabled state
			result = ioctl(disp, DISP_VSYNC_EVENT_EN, (void*)arg);
		}

		// Close the display engine
		close(disp);
	}

	// Return the result
	return result;
}