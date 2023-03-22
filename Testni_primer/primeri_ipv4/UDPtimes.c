#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
	int sockfd, numbytes;  // socekt file descriptor, number of bytes
	socklen_t length; // socket length (length of clinet address)
	struct sockaddr_in saddr, caddr; // server address, client address
	time_t itime;  // time format
	char buf[2048], *tstr; // vars holding the string to be send via TCP

	if(argc != 2) {
		write(0, "Uporaba: UDPtimes vrata\n\0", 25);
		exit(1);
	}

        // create socket    
	if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
	}

	saddr.sin_family = AF_INET; // IPv4 
	saddr.sin_addr.s_addr = INADDR_ANY; // localhost
	saddr.sin_port = htons(atoi(argv[1])); // port converted from ascii to integer

	// binds the socket file description to the actual port (similar to open)
	if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		perror("bind");
	}

	length = sizeof(caddr);

        // start listening on a given port
	while(1) {
	
	        // wait for client's request
		//perror("start waiting...");
		if ((numbytes = recvfrom(sockfd, buf, 1, 0, (struct sockaddr *)&caddr, &length)) < 0) {
			perror("recvfrom");
		}
		//perror("done!");
	
		itime = time(NULL); // get the number of seconds elapsed since 00:00:00 on January 1, 1970
		tstr = ctime(&itime); // convert them to the current date and time
	
	         // send time string (no htonX needed as we are sending characters!!)
		if(sendto(sockfd, tstr, 26, 0, (struct sockaddr *)&caddr, length) < 0) {
			perror("sendto");
		}
	}

	//close(sockfd);
	
	return 0;
}
