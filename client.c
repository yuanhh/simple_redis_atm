#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "include/inet.h"

#define SERVER "127.0.0.1"
#define PORT "7000"

#define OK "OK\n"
#define NO "NO\n"
#define END "END\n"

int main(int argc, char *argv[])
{
    FILE *stream;
    int conn_fd;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char buf[3000] = {0};
    int status = 0;
    int success = 0, fail = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    stream = fopen(argv[1], "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
        conn_fd = active_tcp(SERVER, PORT);
    else
        conn_fd = active_tcp(SERVER, argv[2]);

    while ((nread = getline(&line, &len, stream)) != -1) {

        status = write(conn_fd, line, nread);

        memset(buf, 0, 3000);
        status = read(conn_fd, buf, 3000);
        if (status <= 0)
            break;

        if (strlen(buf) == 3 && !strncmp(buf, OK, 3))
            success++;
        else if (strlen(buf) == 3 && !strncmp(buf, NO, 3))
            fail++;
        else if (strlen(buf) == 4 && !strncmp(buf, END, 4)) {
            memset(buf, 0, 3000);
            while (read(conn_fd, buf, 3000) > 0) {
                printf("%s", buf);
                memset(buf, 0, 3000);
            }
            break;
        } 
    }

    printf("\nsuccess rate : (%d/%d)\n", success, success + fail);

    free(line);
    fclose(stream);
    close(conn_fd);

    return 0;
}
