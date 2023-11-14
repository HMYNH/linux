#include <stdio.h>
#include <unistd.h>

int main(){

	fork();
	printf("%d - line %d\n", getpid(), __LINE__);
	fork();
	printf("%d - line %d\n", getpid(), __LINE__);
	
	return 0;
}
