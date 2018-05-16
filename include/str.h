#ifndef _STR_H
#define _STR_H

#include <stdlib.h>

char **strtok_bychar(const char *cmds_in_line, const char delim, int *cmds_count);
void free_2Darray(char ***array, int count);

#endif
