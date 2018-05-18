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

void basic(char *input, int conn_fd)
{
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char buf[3000] = {0};
    int success = 0, fail = 0;
    int status = 0;

    stream = fopen(input, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while ((nread = getline(&line, &len, stream)) != -1) {

        printf("%s", line);

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
}

void upgrade(char *input, int conn_fd1, int conn_fd2, int conn_fd3)
{
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char buf[3000] = {0};
    int success = 0, fail = 0;
    int status = 0;
    int count = 0;

    stream = fopen(input, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while ((nread = getline(&line, &len, stream)) != -1) {

        if (count % 3 == 0) {
            status = write(conn_fd1, line, nread);

            memset(buf, 0, 3000);
            status = read(conn_fd1, buf, 3000);
            if (status <= 0)
                break;

            if (strlen(buf) == 3 && !strncmp(buf, OK, 3))
                success++;
            else if (strlen(buf) == 3 && !strncmp(buf, NO, 3))
                fail++;
            else if (strlen(buf) == 4 && !strncmp(buf, END, 4)) {
                memset(buf, 0, 3000);
                while (read(conn_fd1, buf, 3000) > 0) {
                    printf("%s", buf);
                    memset(buf, 0, 3000);
                }
                break;
            }
        } else if (count % 3 == 1) {
            status = write(conn_fd2, line, nread);

            memset(buf, 0, 3000);
            status = read(conn_fd2, buf, 3000);
            if (status <= 0)
                break;

            if (strlen(buf) == 3 && !strncmp(buf, OK, 3))
                success++;
            else if (strlen(buf) == 3 && !strncmp(buf, NO, 3))
                fail++;
            else if (strlen(buf) == 4 && !strncmp(buf, END, 4)) {
                memset(buf, 0, 3000);
                while (read(conn_fd2, buf, 3000) > 0) {
                    printf("%s", buf);
                    memset(buf, 0, 3000);
                }
                break;
            }
        } else if (count % 3 == 2) {
            status = write(conn_fd3, line, nread);

            memset(buf, 0, 3000);
            status = read(conn_fd3, buf, 3000);
            if (status <= 0)
                break;

            if (strlen(buf) == 3 && !strncmp(buf, OK, 3))
                success++;
            else if (strlen(buf) == 3 && !strncmp(buf, NO, 3))
                fail++;
            else if (strlen(buf) == 4 && !strncmp(buf, END, 4)) {
                memset(buf, 0, 3000);
                while (read(conn_fd3, buf, 3000) > 0) {
                    printf("%s", buf);
                    memset(buf, 0, 3000);
                }
                break;
            }
        }
    }

    printf("\nsuccess rate : (%d/%d)\n", success, success + fail);

    free(line);
    fclose(stream);
}

int main(int argc, char *argv[])
{
    int conn_fd1, conn_fd2, conn_fd3;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        conn_fd1 = active_tcp(SERVER, PORT);
        basic(argv[1], conn_fd1);
        close(conn_fd1);
    } else if (argc == 3) {
        conn_fd1 = active_tcp(SERVER, argv[2]);
        basic(argv[1], conn_fd1);
        close(conn_fd1);
    } else if (argc == 5) {
        conn_fd1 = active_tcp(SERVER, argv[2]);
        conn_fd2 = active_tcp(SERVER, argv[3]);
        conn_fd3 = active_tcp(SERVER, argv[4]);
        upgrade(argv[1], conn_fd1, conn_fd2, conn_fd3);
        close(conn_fd1);
        close(conn_fd2);
        close(conn_fd3);
    }

    return 0;
}
