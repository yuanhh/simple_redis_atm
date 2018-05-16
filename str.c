#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char **strtok_bychar(const char *cmds_in_line, const char delim, int *cmds_count)
{
    char **result = 0;
    char *tmp = strdup(cmds_in_line);
    char *cptr = tmp;
    char *token = 0;
    char *rest_of_cmd = NULL;
    int index = 0;
    int flag = 0;

    *cmds_count = 0;
    while (*cptr) {
        if ((*cptr != delim) && (flag == 0)) {
            (*cmds_count)++;
            flag = 1;
        } else if ((*cptr == delim) && (flag == 1))
            flag = 0;
        cptr++;
    }

    while (result == NULL)
        result = malloc(sizeof(char*) * ((*cmds_count) + 1));

    token = strtok_r(tmp, &delim, &rest_of_cmd);
    for (index = 0; index < *cmds_count; index ++) {
        result[index] = strdup(token);
        token = strtok_r(rest_of_cmd, &delim, &rest_of_cmd); 
    }
    result[index] = 0;
    free(tmp);
    return result;
}

void free_2Darray(char ***array, int count)
{
    int i = 0;

    for (i = 0; i < count; i++) {
        if ((*array)[i]) {
            free((*array)[i]);
            (*array)[i] = 0;
        }
    }
	*array = 0;
}
