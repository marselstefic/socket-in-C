#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define MAXDATASIZE 1024

struct myStruct {
    int metapodatki; //4
    int dolzinaImeZbirke; //4
    int velikostZbirke; //4
    int hashZbirke; //4
};


int main(int argc, char *argv[]) {
	char imeZbirke[MAXDATASIZE];
    char buffer[MAXDATASIZE];
    struct myStruct *fileInfo = malloc(sizeof(struct myStruct));
	int sockfd, newfd; // socekt file descriptor, new file descriptor
	socklen_t length;  // socket length (length of clinet address)
	struct sockaddr_in saddr, caddr; // server address, client address
	time_t itime; // time format
	char *tstr; // var holding the string to be send via TCP
	char path[1024] = "/home/naptus/faks/NPO/Vaja2/target/";

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

	FILE* fp;

		// accept new client (wait for him!)
		if((newfd = accept(sockfd, (struct sockaddr *)&caddr, &length)) > 0) {
			perror("accept");
			recv(newfd, fileInfo, sizeof(struct myStruct), 0);
			recv(newfd, &imeZbirke, MAXDATASIZE, 0);
			printf("%s", imeZbirke);
			strcat(path, imeZbirke);
			fp = fopen(path, "a");

		// if(recv(newfd, &buffer, MAXDATASIZE, 0) <= 0) {
		// 	fprintf(fp, "%s", buffer);
		// 	perror("send");
		// }

		close(newfd); // close socket
	}
		


	
	return 0;
}