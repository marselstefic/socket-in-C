#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>

#define MAXDATASIZE 1024

struct myStruct {
    int metapodatki; //4
    int dolzinaImeZbirke; //4
    int velikostZbirke; //4
    int hashZbirke; //4
};

void* worker (void* socketNew) {
	int newfd = *(int*)socketNew;
	FILE* fp;
	size_t sizeRead;
	char buffer[MAXDATASIZE];
    struct myStruct fileInfo;
	char path[1024] = "Transferred_";

	recv(newfd, &fileInfo, sizeof(fileInfo), 0);
	char imeZbirke[fileInfo.dolzinaImeZbirke + 1];
	recv(newfd, &imeZbirke, sizeof(imeZbirke), 0);

	printf("fileInfo->metapodatki: %x\n", fileInfo.metapodatki);
	printf("fileInfo->dolzinaImeZbirke: %d\n", fileInfo.dolzinaImeZbirke);
	printf("fileInfo->velikostZbirke: %d\n", fileInfo.velikostZbirke);
	printf("fileInfo->hashZbirke: %d\n", fileInfo.hashZbirke);
	printf("fileInfo->imeZbirke: %s\n", imeZbirke);

	strcat(path, imeZbirke);
	fp = fopen(path, "a");
	printf("%s", path);

	while((sizeRead = recv(newfd, buffer, MAXDATASIZE, 0)) > 0) {
		if(fwrite(buffer, 1, sizeRead, fp) != sizeRead) {
			perror("send");
			printf("%d", 4);
		}
		
	}
	close(newfd); // close socket
	fclose(fp);
}


int main(int argc, char *argv[]) {
	int sockfd, newfd; // socekt file descriptor, new file descriptor
	socklen_t length;  // socket length (length of clinet address)
	struct sockaddr_in saddr, caddr; // server address, client address
	time_t itime; // time format
	char *tstr; // var holding the string to be send via TCP
	

	if(argc != 2) {
		write(0, "Uporaba: TCPtimes vrata (vrata 0-1024 so rezervirana za jedro)\n\0", 25);
		exit(1);
	}
	
	
	// create socket
	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
	}

	saddr.sin_family = AF_INET; // IPv4 
	saddr.sin_addr.s_addr = INADDR_ANY; // localhost
	saddr.sin_port = htons(atoi(argv[1])); // port converted from ascii to integer

        // binds the socket file description to the actual port (similar to open)
	if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("bind");
	}

	// start listening on the given port
	if(listen(sockfd, 0) < 0) {
		perror("listen");
	}

	length = sizeof(caddr); // length of client address
		
	
	while(1){
		if((newfd = accept(sockfd, (struct sockaddr *)&caddr, &length)) < 0) {
			perror("accept");
		}
		pthread_t threadWorker;
		pthread_create(&threadWorker, NULL, worker, (void*)&newfd);
		pthread_detach(threadWorker);
	}
		
	return 0;
}