#include <stdlib.h> /* exit*/
#include <stdio.h>  /* fprintf */
#include <netdb.h>  /* addrinfo */
#include <unistd.h> /* close */
#include <fcntl.h>  /* fcntl */
#include <string.h> /* memset */
#include <errno.h>  /* errno */

struct addrinfo* getaddrlist(
        const char *node,
        const char *service,
        int socktype,
        int flags,
        int protocol) {
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;                  /* Allow IPv4 only */
    hints.ai_socktype = socktype;               /* STREAM/DGRAM socket */
    hints.ai_flags = hints.ai_flags | flags;    /* For wildcard IP address */
    hints.ai_protocol = protocol;               /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    status = getaddrinfo(node, service, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return NULL;
    }
    return res;
}

int create_socket(struct addrinfo *laddrinfo, int active) {
    struct  addrinfo *rp;
    int     sfd;
    int     optval = 1;

    for (rp = laddrinfo; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
        
        if (active) {
            if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
                break;
        } else {
            if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
                break;
        }
        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
    return sfd;
}

int tcp_socket(const char *host, const char *service, int active) {
    struct addrinfo *laddrinfo;
    int sfd;

    // if node is not NULL, AI_PASSIVE flag is ignored
    laddrinfo = getaddrlist(host, service, SOCK_STREAM,
            AI_PASSIVE, IPPROTO_TCP);

    sfd = create_socket(laddrinfo, active);


    freeaddrinfo(laddrinfo);

    if (!active && listen(sfd, 10) == -1) {
        fprintf(stderr, "Could not listen\n");
        exit(EXIT_FAILURE);
    }

    return sfd;
}

int passive_tcp(const char *host, const char *service) {
    return tcp_socket(host, service, 0);
}
int active_tcp(const char *host, const char *service) {
    return tcp_socket(host, service, 1);
}

int udp_socket(const char *host, const char *service, int active) {
    struct addrinfo *laddrinfo;
    int sfd;

    // if node is not NULL, AI_PASSIVE flag is ignored
    laddrinfo = getaddrlist(host, service, SOCK_DGRAM,
            AI_PASSIVE, IPPROTO_UDP);

    sfd = create_socket(laddrinfo, active);

    freeaddrinfo(laddrinfo);

    return sfd;
}

int passive_udp(const char *host, const char *service) {
    return udp_socket(host, service, 0);
}
int active_udp(const char *host, const char *service) {
    return udp_socket(host, service, 1);
}

int accept_tcp(int sfd) {
    int conn_fd;
    socklen_t len = sizeof(struct sockaddr);
    static struct sockaddr_in peer;

    memset(&peer, 0, len);
    conn_fd = accept(sfd, (struct sockaddr *)&peer, &len);
    if (conn_fd < 0) {
        if (errno != EINTR) {
            perror("accept");
        }
    }
    return conn_fd;
}

struct sockaddr resolve_host(const char *host, const char *service) {
    struct addrinfo *raddrinfo;
    struct sockaddr sa;

    raddrinfo = getaddrlist(host, service, SOCK_DGRAM, AI_CANONNAME, IPPROTO_UDP);

    if (raddrinfo == NULL) {
        fprintf(stderr, "client: failed to resolve host name\n");
        exit(EXIT_FAILURE);
    }
    sa = *(raddrinfo->ai_addr);

    freeaddrinfo(raddrinfo);

    return sa;
}

void set_nonblock(int sfd) {
    int flags;
    int rv;
    rv = fcntl(sfd, F_GETFL, &flags);
    if (rv < 0) {
        fprintf(stderr, "Could not get flags\n");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    rv = fcntl(sfd, F_SETFL, &flags);
    if (rv < 0) {
        fprintf(stderr, "Could not set flags\n");
        exit(EXIT_FAILURE);
    }
}
