#include<stdio.h>

char *atox(const char *s){
	static char ret[1024] = {0};
	char *p1 = (char *)s;
	char *p2 = ret;
	for(; *p1; p1++, p2 += 5){
		sprintf(p2,"%#x ",*p1);
	}
	return ret;
}

int main(){

	printf("%s\n", atox("ABCD"));

	return 0;
}
