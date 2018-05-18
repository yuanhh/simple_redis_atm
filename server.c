#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/inet.h"
#include "include/process.h"

#define PORT "7000"

int main(int argc, char *argv[])
{
    int server_fd, conn_fd;
    char *ATM[2] = {"atm", 0};

    if (argc == 1)
        server_fd = passive_tcp(NULL, PORT);
    else
        server_fd = passive_tcp(NULL, argv[1]);

    setenv("PATH", ".", 1);

    while (1) {
        conn_fd = accept_tcp(server_fd);
        if (conn_fd < 0) break;

        process(ATM, conn_fd, conn_fd, 2, 1);

        close(conn_fd);
    }
    close(server_fd);

    return 0;
}
