#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t sem;

/* 模拟窗口 线程 */
void *threadHandler(void *arg){
	long id = (long)arg;
	int sem_value;

	while(1){
		if( -1 == sem_trywait(&sem) ){
			break;
		}
		if( -1 == sem_getvalue(&sem, &sem_value) ){
			perror("sem_getvalue");
		}
		printf("=== windows %c --- client %-2d business begin.===\n", (char)('A' + id), 20 - sem_value);
		sleep(3);
		printf("### windows %c --- client %-2d business over.###\n", (char)('A' + id), 20 - sem_value);
	}
	pthread_exit(NULL);
}

/* 主线程 */
int main(int argc, char **argv){

	pthread_t pt[3];

	sem_init(&sem, 0, 20);

	for(long i = 0; i < 3; i++){
		if( pthread_create(pt + i, NULL, threadHandler, (void *)i) ){
			perror("pthread_create");
			return -1;
		}
	}

	for(int i = 0; i < 3; i++){
		pthread_join(pt[i],NULL);
	}

	return 0;
}

