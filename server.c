#include <stdio.h>
#include <unistd.h>

#include "include/inet.h"
#include "include/process.h"

#define PORT "7000"

int main(void)
{
    int server_fd, conn_fd;
    char *ATM[2] = {"atm", 0};

    server_fd = passive_tcp(NULL, PORT);

    while (1) {
        conn_fd = accept_tcp(server_fd);
        if (conn_fd < 0) break;

        process(ATM, conn_fd, conn_fd, conn_fd, 1);

        close(conn_fd);
    }
    close(server_fd);
}
