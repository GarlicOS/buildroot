#ifndef INPUT_H
#define INPUT_H

#define AUDIO_CONTROL_PIPE "/tmp/audio_control"
#define AUDIO_RESPONSE_PIPE "/tmp/audio_response"

const char * get_input_device_path(const char * input_device_name);
const char * wait_for_input_device(const char * input_device_name);

#endif