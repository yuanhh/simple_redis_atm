#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
            if (ch == '\n')
                break;
            if (ch == '\r')
                continue;

            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }
        }
    }

    *buf = '\0';
    return totRead;
}

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
