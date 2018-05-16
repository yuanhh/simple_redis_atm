#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "include/inet.h"

#define SERVER "127.0.0.1"
#define PORT "7000"

int main(int argc, char *argv[])
{
    FILE *stream;
    int conn_fd;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char buf[1024] = {0};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    stream = fopen(argv[1], "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    conn_fd = active_tcp(SERVER, PORT);

    while ((nread = getline(&line, &len, stream)) != -1) {
        write(conn_fd, line, nread);
        memset(buf, 0, 1024);
        read(conn_fd, buf, 1024);
        printf("%s\n", buf);
    }

    free(line);
    fclose(stream);
    close(conn_fd);
}
