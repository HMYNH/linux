#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>

#define ERROR_HANDLE(FUNC)	{ fprintf(stderr,"line(%d) - \"%s\" : %s\n", \
								__LINE__, FUNC, strerror(errno)); _exit(-1); }
#define OPCODE(BUF)			(*(unsigned short *)BUF)
#define BLOCK_NUM(BUF)		(*((unsigned short *)BUF + 1))

enum opcode{
	RRQ = 1,WRQ,DATA,ACK,ERROR,
};

/* 全局变量 */
int               sockfd;
FILE             *fp;
char             *filename;
char             *ipv4;
unsigned short    port;
struct sockaddr_in target_addr;
socklen_t         addr_len;

/* 函数声明 */
int upload();
int download();
int request(enum opcode op);

/* getopt_long */
/* --port=??? --ip=????? --filename=????? --download */
/* 主函数 */
int main(int argc, char **argv){

	port = 69;
	if( argc < 4 ){
		printf("< usage > : %s [ipv4] [upload/download] [filename] [port]\n", argv[0]);
		printf("            The argument [port] default value is 69 if not\n");
		printf("            suppurt, you can input new value to specify.\n\n");
		return -1;
	}
	if( argv[4] ){
		port = atoi(argv[4]);
	}
	ipv4     = argv[1];
	filename = argv[3];
	addr_len = sizeof(struct sockaddr_in);
	if( -1 == (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) ){
		ERROR_HANDLE("socket");
	}
	if( strcmp(argv[2],"upload") && strcmp(argv[2],"download") ){
		printf("Invalid transferm mode name.\n");
		return -1;
	}
	return ( !strcmp(argv[2],"upload") ? upload() : download() );
}

int upload(){
	unsigned char data[516] = {0};
	ssize_t readlen;
	unsigned short block_num = 1;
	unsigned short recv_num = 0;
	
	request(WRQ);
	recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&target_addr, &addr_len);
	if( ntohs(OPCODE(data)) == ACK && ntohs(BLOCK_NUM(data)) == 0 ){
		if( !(fp = fopen(filename, "rb")) ){
			ERROR_HANDLE("fopen");
		}
		recv_num = block_num;
		while(1){
			OPCODE(data) = htons(DATA), BLOCK_NUM(data) = htons(block_num) ;
			if( block_num == recv_num ){
				readlen = fread(data + 4, 1, 512, fp);
			}
			sendto(sockfd, data, 4 + readlen, 0, (const struct sockaddr *)&target_addr, addr_len);
			recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&target_addr, &addr_len);
			printf("[OPCODE : %d | BLOCK_NUM : %-5d]\n", ntohs(OPCODE(data)), ntohs(BLOCK_NUM(data)));
			if( ntohs(OPCODE(data)) == ERROR ){
				printf("ERRMSG : %s\n", (char *)data + 4);break;
			}else if( ntohs(OPCODE(data)) == ACK && ntohs(BLOCK_NUM(data)) == block_num ){
				block_num++;
				recv_num = block_num;
			}else if( ntohs(BLOCK_NUM(data)) - block_num == 1 ){
				recv_num--;
			}
			if( readlen < 512 ){
				printf("File \"%s\" upload completed.\n", basename(filename));
				break;
			}
		}
	}else{
		printf("ERRMSG : %s\n", (char *)data + 4);
	}
	return 0;
}
int download(){
	unsigned char data[516] = {0};
	ssize_t recvlen;
	unsigned short block_num = 0;
	request(RRQ);
	if( !(fp = fopen(filename, "wb")) ){
		ERROR_HANDLE("fopen");
	}
	while(1){
		recvlen = recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&target_addr, &addr_len);
		printf("[OPCODE : %d | BLOCK : %-5d | DATA SIZE : %-3ld]\n" \
			,ntohs(OPCODE(data)), ntohs(BLOCK_NUM(data)),  recvlen - 4);
		if( ntohs(BLOCK_NUM(data)) == block_num ){
			printf("Retransfer \n");
			//sendto(sockfd, data, 4, 0, (const struct sockaddr *)&target_addr, addr_len);
			continue;
		}
		else if( ntohs(OPCODE(data)) == DATA ){
			fwrite(data + 4, 1, recvlen - 4, fp);
			OPCODE(data) = htons(ACK);
			sendto(sockfd, data, 4, 0, (const struct sockaddr *)&target_addr, addr_len);
			block_num = ntohs(BLOCK_NUM(data));
		}else if( ntohs(OPCODE(data)) == ERROR ){
			printf("ERRMSG : %s\n", (char *)data + 4);break;
		}
		if( recvlen < 516 ){
			printf("File \"%s\" download completed.\n", basename(filename));
			break;
		}
	}
	return 0;
}
int request(enum opcode op){
	unsigned char rqst[1032] = {0};
	sprintf((char *)rqst ,"%c%c%s%c%s", 0x00, op, filename, 0x00, "octet");
	memset(&target_addr, 0x00, sizeof(target_addr));
	target_addr.sin_family      = AF_INET;
	target_addr.sin_port        = htons(port);
	target_addr.sin_addr.s_addr = inet_addr(ipv4);
	if( sendto(sockfd, rqst, 9 + strlen(filename), 0, \
				(const struct sockaddr *)&target_addr, addr_len) < 0 ){
		ERROR_HANDLE("sendto");
	}
	return 0;
}

