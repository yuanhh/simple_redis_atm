#ifndef _INET_H
#define _INET_H

int passive_tcp(const char *host, const char *service);
int active_tcp(const char *host, const char *service);
int passive_udp(const char *host, const char *service);
int active_udp(const char *host, const char *service);
int accept_tcp(int sfd);
struct sockaddr resolve_host(const char* host, const char *service);
void set_nonblock(int sfd);

#endif
