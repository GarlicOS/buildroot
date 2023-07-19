#ifndef PROCESS_H
#define PROCESS_H

#include "gui.h"

/**
 * @brief Runs the given process, waits for it to exit and returns its exit code.
 */
int process_run(struct gui_context * context, const char * args[], int replace_parent);

#endif