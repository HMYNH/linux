#ifndef _FDS_MANAGE_H
#define _FDS_MANAGE_H

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* 函数声明 */
void fdsInit(fd_set *p_fds);
void addUser(int sockfd, fd_set *p_fds);
void delUser(int sockfd, fd_set *p_fds);
void copyFds(fd_set *target);

#endif /* _FDS_MANAGE_H */ 