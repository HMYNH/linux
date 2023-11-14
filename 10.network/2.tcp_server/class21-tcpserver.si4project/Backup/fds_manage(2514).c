#include "fds_manage.h"

/* 全局变量 */
fd_set fds;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void fdsInit(fd_set *p_fds){
	FD_ZERO(p_fds);
}

void addUser(int sockfd, fd_set *p_fds){
	pthread_mutex_lock(&mutex);
	FD_SET(sockfd, p_fds);
	pthread_mutex_unlock(&mutex);
}

void delUser(int sockfd, fd_set *p_fds){
	pthread_mutex_lock(&mutex);
	FD_CLR(sockfd, p_fds);
	pthread_mutex_unlock(&mutex);
}

void copyFds(fd_set *target){
	pthread_mutex_lock(&mutex);
	*target = fds;
	pthread_mutex_unlock(&mutex);
}
