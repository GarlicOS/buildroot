#ifndef POCKET3_H
#define POCKET3_H

// The input device paths
#define POCKET3_GAMEPAD "/dev/ttyS0"
#define POCKET3_RUMBLE_MOTOR "/dev/input/event0"
#define POCKET3_TOUCHSCREEN "/dev/input/event1"
#define POCKET3_GPIO_KEYS "/dev/input/event2"
#define POCKET3_HEADPHONE_JACK "/dev/input/event3"
#define POCKET3_HEADPHONE_KEYBOARD "/dev/input/event4"
// Note: pocket3 does not have a gamepad_touchscreen event

// System buttons (gpio-keys)
#define POCKET3_SCANCODE_VOLUME_MINUS KEY_VOLUMEDOWN
#define POCKET3_SCANCODE_VOLUME_PLUS KEY_VOLUMEUP
#define POCKET3_SCANCODE_POWER KEY_POWER

// Retroid Pocket 2+ Serial Gamepad Scancodes
#define POCKET3_SCANCODE_HOME 102
#define POCKET3_SCANCODE_A 48
#define POCKET3_SCANCODE_B 49
#define POCKET3_SCANCODE_X 51
#define POCKET3_SCANCODE_Y 52
#define POCKET3_SCANCODE_L1 54
#define POCKET3_SCANCODE_R1 55
#define POCKET3_SCANCODE_L2 56
#define POCKET3_SCANCODE_R2 57
#define POCKET3_SCANCODE_SELECT 58
#define POCKET3_SCANCODE_START 59
#define POCKET3_SCANCODE_L3 61
#define POCKET3_SCANCODE_R3 62
#define POCKET3_SCANCODE_DPAD_UP 32
#define POCKET3_SCANCODE_DPAD_DOWN 33
#define POCKET3_SCANCODE_DPAD_LEFT 34
#define POCKET3_SCANCODE_DPAD_RIGHT 35

// XBOX360 gamepad axis (retrogame_joypad)
#define POCKET3_AXIS_LEFT_ANALOG_HORIZONTAL 0
#define POCKET3_AXIS_LEFT_ANALOG_VERTICAL 1
#define POCKET3_AXIS_RIGHT_ANALOG_HORIZONTAL 4
#define POCKET3_AXIS_RIGHT_ANALOG_VERTICAL 5

// Analog stick axis range, fuzz and flat values, unused
#define POCKET3_ANALOG_VALUE_MIN 800
#define POCKET3_ANALOG_VALUE_MAX 2902
#define POCKET3_ANALOG_VALUE_FUZZ 64
#define POCKET3_ANALOG_VALUE_FLAT 32

// Dpad axis range, fuzz and flat values (Retroid Serial Gamepad)
#define POCKET3_DPAD_VALUE_MIN -1
#define POCKET3_DPAD_VALUE_MAX 1
#define POCKET3_DPAD_VALUE_FUZZ 0
#define POCKET3_DPAD_VALUE_FLAT 0

void merge_pocket3_inputs(int merged_gamepad);

#endif