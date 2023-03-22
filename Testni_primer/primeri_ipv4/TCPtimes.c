#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

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

	while(1) {

		// accept new client (wait for him!)
		//perror("start waiting...");
		if((newfd = accept(sockfd, (struct sockaddr *)&caddr, &length)) < 0) {
			perror("accept");
		}
		//perror("done!");
                
		itime = time(NULL); // get the number of seconds elapsed since 00:00:00 on January 1, 1970
		tstr = ctime(&itime); // convert them to the current date and time
	
	        // send time string (no htonX needed as we are sending characters!!)
		if(send(newfd, tstr, 26, 0) < 0) {
			perror("send");
		}

		close(newfd); // close socket
	}

	//close(sockfd);
	
	return 0;
}
