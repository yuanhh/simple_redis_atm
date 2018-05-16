#ifndef _PROCESS_H
#define _PROCESS_H

void dup_fd(int infd, int outfd, int errfd);
int process(char *const *cmd, int infd, int outfd, int errfd, int block);

#endif
