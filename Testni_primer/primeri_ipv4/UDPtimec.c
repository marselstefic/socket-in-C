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
	struct hostent *he;  // pointer to the structure hostent (returned by gethostbyname) 
	struct sockaddr_in their_addr; // server address

	socklen_t length = sizeof(their_addr);	

	if (argc != 3) {
		write(0,"Uporaba: UDPtimec ime vrata\n", 29);
		exit(1);
	}
	
	// get the server IP address
	if ((he=gethostbyname(argv[1])) == NULL) { 
		herror("gethostbyname"); 
		exit(1);
	}
	
	// create socket
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	their_addr.sin_family = AF_INET; // family to Address Family InterNET
	their_addr.sin_port = htons(atoi(argv[2])); // get server's port number - should be checked for input errors (not a number, etc.)
	their_addr.sin_addr = *((struct in_addr *)he->h_addr); // server IP address from hostent structure pointed to by he variable...
	memset(&(their_addr.sin_zero), '\0', 8); // for conversion between structures only, a trick to ensure pointer casting...
	
	
	// send to the server your "empty" request... (no bind or connect necessary... client IP and port automatically from IP/UDP protocol)
	if(sendto(sockfd, "\0", 1, 0, (struct sockaddr *)&their_addr, length) < 0) {
		perror("sendto");
	}

	// receive the answer
	if ((numbytes=recvfrom(sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr *)&their_addr, &length)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	
	buf[numbytes] = '\0'; // all strings in c ends with \0 (see strlen)
	
	// print received string to standard output
	write(0, buf, numbytes);

	// close socket
	close(sockfd);
	
	return 0;
} 
