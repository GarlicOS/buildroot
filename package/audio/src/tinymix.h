#ifndef TINYMIX_H
#define TINYMIX_H

#include <tinyalsa/asoundlib.h>

int set_values(struct mixer * mixer, const char * control, char ** values, unsigned int num_values);
int set_value(struct mixer * mixer, const char * control, char * value);
int get_int_value(struct mixer * mixer, const char * control, int * value, int * min, int * max);

#endif
