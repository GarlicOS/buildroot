#ifndef PROCESS_H
#define PROCESS_H

/**
 * @brief Runs the given process, waits for it to exit and returns its exit code.
 */
int process_run(const char * args[], int replace_parent);

#endif