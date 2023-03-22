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
    int sockfd; // socket file descriptor (oprimek)
    struct addrinfo hints, *servinfo, *p; // server address
    int rv;
    int numbytes;
    char buf[MAXBUFLEN]; // buffer for receiving
    char s[INET6_ADDRSTRLEN]; // buffer for IP address (for printing it out to stdout)

    if (argc != 3) {
        fprintf(stderr,"usage: UDPtimecIPv6 hostname port\n");
        exit(1);
    }

    // fill in the hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; //ce pri IPv6 nagaja localhost (::1) nadomesti AF_UNSPEC z AF_INET ;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    

    // loop through all the results and make a socket (no binding since we use UDP)
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("UDPclient: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "UDPclient: failed to bind socket\n");
        return 2;
    }
     
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("UDPclient: connecting to %s\n", s);

    // send to the server your "empty" request... (no bind or connect necessary... client IP and port automatically from IP/UDP protocol)
    char a='\0';
    if ((numbytes = sendto(sockfd, &a, strlen(&a), 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("UDPclient: sendto");
        exit(1);
    }
    
    // receive the answer
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,p->ai_addr, &p->ai_addrlen)) == -1) {
        perror("recvfrom");
        exit(1);
    }
     
    // print received string to standard output
    buf[numbytes] = '\0'; // all strings in c ends with \0 (see strlen)
    write(0, buf, numbytes);

    // free memory and close the socket
    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}
