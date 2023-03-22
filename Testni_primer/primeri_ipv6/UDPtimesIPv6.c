#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>


#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd;// socket file descriptor (oprimek)
    struct addrinfo hints, *servinfo, *p; // server address
    int rv;
    int numbytes;
    socklen_t addr_length; // address length (different for IPv4 and IPv6)
    struct sockaddr_storage their_addr; // client address
    char buf[MAXBUFLEN]; // buffer for receiving
    char s[INET6_ADDRSTRLEN]; // buffer for IP address (for printing it out to stdout)
    time_t itime; // time format
    char *tstr; // var holding the string to be send via UDP

    if (argc != 2) {
        fprintf(stderr,"usage: UDPtimesIPv6 port\n");
        exit(1);
    }
    
    // fill in the hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("UDPserver: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("UDPserver: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "UDPserver: failed to bind socket\n");
        return 2;
    }

    // free memory (we are bind so we do not need the list of server's addresses anymore)
    freeaddrinfo(servinfo);

    while(1) {  // main accept() loop

    	printf("UDPserver: waiting to recvfrom...\n");

        //receive the request
    	addr_length = sizeof(their_addr);
    	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_length)) == -1) {
       		perror("recvfrom");
        	exit(1);
    	}

        // print the connection details
    	printf("UDPserver: got packet from %s\n",
        	inet_ntop(their_addr.ss_family,	get_in_addr((struct sockaddr *)&their_addr), s, sizeof s)); // Convert IP addresses to human-readable form 
    	printf("UDPserver: packet is %d bytes long\n", numbytes);
        
        // print received string to standard output
    	buf[numbytes] = '\0';
    	printf("UDPserver: packet contains \"%s\"\n", buf);

    	itime = time(NULL); // get the number of seconds elapsed since 00:00:00 on January 1, 1970
    	tstr = ctime(&itime); // convert them to the current date and time

    	// send time string in respond (no htonX needed as we are sending characters!!)
    	if ((numbytes = sendto(sockfd, tstr, 26, 0,(struct sockaddr *)&their_addr, addr_length)) == -1)
        	perror("sendto: ");
              	
    }
    
    //close the socket    
    close(sockfd);
    
    return 0;
}
