#ifndef SPREADTRUM_H
#define SPREADTRUM_H

int init_spreadtrum_agdsp();
int init_spreadtrum_alsa();
void spreadtrum_update(const char * headphone_jack_device_name, uint16_t jack_event_type, uint16_t jack_event_code);

#endif