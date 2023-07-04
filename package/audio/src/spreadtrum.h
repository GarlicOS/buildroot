#ifndef SPREADTRUM_H
#define SPREADTRUM_H

int init_spreadtrum_agdsp();
int init_spreadtrum_alsa();
void restore_brightness();
void spreadtrum_update(const char * headphone_jack_device_name, uint16_t jack_event_type, uint16_t jack_event_code, const char * volume_control_device_name, uint16_t volume_event_type, uint16_t volume_down_event_code, uint16_t volume_up_event_code, const char * gamepad_device_name, uint16_t modifier_event_type, uint16_t modifier_event_code);

#endif
