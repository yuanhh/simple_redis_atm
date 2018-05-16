#ifndef _STR_H
#define _STR_H

#include <stdlib.h>

ssize_t readLine(int fd, void *buffer, size_t n);
char **strtok_bychar(const char *cmds_in_line, const char delim, int *cmds_count);
void free_2Darray(char ***array, int count);

#endif
