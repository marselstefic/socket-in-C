#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAXDATASIZE 1024 //število prebranih zlogov

int main(int argc, char *argv[]) {

	int sockfd, numbytes;  // socekt file descriptor, new file descriptor
	char buf[MAXDATASIZE]; // array holding the string to be received via TCP
	struct hostent *he;    // pointer to the structure hostent (returned by gethostbyname) 
	struct sockaddr_in their_addr; // server address
	
	if (argc != 3) {
		write(0,"Uporaba: TCPtimec ime vrata\n\0", 29);
		exit(1);
	}
	
	// get the server IP address
	if ((he = gethostbyname(argv[1])) == NULL) { 
		herror("gethostbyname");  // prints string + value of h_error variable [HOST_NOT_FOUND || NO_ADDRESS or NO_DATA || NO_RECOVERY || TRY_AGAIN]
		exit(1);		  
	}
	
	// create socket
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	their_addr.sin_family = AF_INET; // family to Address Family InterNET
	their_addr.sin_port = htons(atoi(argv[2])); // get server's port number - should be checked for input errors (not a number, etc.)
 	their_addr.sin_addr = *((struct in_addr *)he->h_addr); // server's IP address from hostent structure pointed to by he variable...
	memset(&(their_addr.sin_zero), '\0', 8); // for conversion between structures only, a trick to ensure pointer casting...
	
	
	// connect to the server... (no bind necessary as we are connecting to remote host... Kernel will find a free port for us and will bind it to sockfd)
	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}

	// as long there is soething to read...
	while(numbytes != 0) {
	
		// receive numbytes from server...
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		
		buf[numbytes] = '\0'; // all strings in c ends with \0 (see strlen)
		
		// print received string to standard output
		write(0, buf, numbytes);
	}
	
	// close socket
	close(sockfd);
	
	return 0;
} 
