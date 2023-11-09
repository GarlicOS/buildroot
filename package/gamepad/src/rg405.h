#ifndef RG405_H
#define RG405_H

// The input device paths
#define RG405_TOUCHSCREEN "/dev/input/event0"
#define RG405_GPIO_KEYS "/dev/input/event1"
#define RG405_GAMEPAD "/dev/input/event2"
#define RG405_GAMEPAD_TOUCHSCREEN "/dev/input/event3"
#define RG405_RUMBLE_MOTOR "/dev/input/event4"
#define RG405_HEADPHONE_JACK "/dev/input/event5"
#define RG405_HEADPHONE_KEYBOARD "/dev/input/event6"

// System buttons (gpio-keys)
#define RG405_SCANCODE_VOLUME_MINUS KEY_VOLUMEDOWN
#define RG405_SCANCODE_VOLUME_PLUS KEY_VOLUMEUP
#define RG405_SCANCODE_POWER KEY_POWER

// XBOX360 gamepad buttons (retrogame_joypad)
#define RG405_SCANCODE_HOME KEY_BACK
#define RG405_SCANCODE_A BTN_SOUTH
#define RG405_SCANCODE_B BTN_EAST
#define RG405_SCANCODE_X BTN_NORTH
#define RG405_SCANCODE_Y BTN_WEST
#define RG405_SCANCODE_L1 BTN_TL
#define RG405_SCANCODE_R1 BTN_TR
#define RG405_SCANCODE_L2 BTN_TL2
#define RG405_SCANCODE_R2 BTN_TR2
#define RG405_SCANCODE_SELECT BTN_SELECT
#define RG405_SCANCODE_START BTN_START
#define RG405_SCANCODE_L3 BTN_THUMBL
#define RG405_SCANCODE_R3 BTN_THUMBR

// XBOX360 gamepad axis (retrogame_joypad)
#define RG405_AXIS_LEFT_ANALOG_HORIZONTAL 0
#define RG405_AXIS_LEFT_ANALOG_VERTICAL 1
#define RG405_AXIS_RIGHT_ANALOG_HORIZONTAL 2
#define RG405_AXIS_RIGHT_ANALOG_VERTICAL 5
#define RG405_AXIS_DPAD_HORIZONTAL 16
#define RG405_AXIS_DPAD_VERTICAL 17

// Analog stick axis range, fuzz and flat values (retrogame_joypad)
#define RG405_ANALOG_VALUE_MIN -1800
#define RG405_ANALOG_VALUE_MAX 1800
#define RG405_ANALOG_VALUE_FUZZ 64
#define RG405_ANALOG_VALUE_FLAT 32

// Dpad axis range, fuzz and flat values (retrogame_joypad)
#define RG405_DPAD_VALUE_MIN -1
#define RG405_DPAD_VALUE_MAX 1
#define RG405_DPAD_VALUE_FUZZ 0
#define RG405_DPAD_VALUE_FLAT 0

void merge_rg405_inputs(int merged_gamepad);

#endif
