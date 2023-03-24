#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXDATASIZE 1024 //število prebranih zlogov

struct myStruct {
    int metapodatki; //4
    int dolzinaImeZbirke; //4
    int velikostZbirke; //4
    int hashZbirke; //4
};

int main(int argc, char *argv[]) {

	char imeZbirke[1024]; //dolzinaImeZbrike+1
    char buffer[1024]; //1040   
	struct stat file;
	FILE* fp = fopen (argv[3], "r");
	int fd = open(argv[3], O_RDONLY);
	struct myStruct *fileInfo = malloc(sizeof(struct myStruct));

	//Fill OUT STRUCT
	//metapodatki && velikostZbirke
	stat(argv[3], &file);
	int is_file = S_ISREG(file.st_mode);
	if (is_file == 1) {
		fileInfo->metapodatki = 0x8000000;
		fileInfo->velikostZbirke = file.st_size;

	} else {
		fileInfo->metapodatki = 0x40000000;
		fileInfo->velikostZbirke = 0;
	}

	//dolzinaImeZbirke
	fileInfo->dolzinaImeZbirke = strlen(argv[3])+1;

	//imeZbirke
	strcpy(imeZbirke, argv[3]);

	printf("fileInfo->metapodatki: %d\n", fileInfo->metapodatki);
	printf("fileInfo->dolzinaImeZbirke: %d\n", fileInfo->dolzinaImeZbirke);
	printf("fileInfo->velikostZbirke: %d\n", fileInfo->velikostZbirke);
	printf("fileInfo->imeZbirke: %s\n", imeZbirke);

	int sockfd, numbytes;  // socekt file descriptor, new file descriptor
	char buf[MAXDATASIZE]; // array holding the string to be received via TCP
	struct hostent *he;    // pointer to the structure hostent (returned by gethostbyname) 
	struct sockaddr_in their_addr; // server address
	
	if (argc != 4) {
		write(0,"Uporaba: TCPtimec <ime> <vrata> <file|directory> \n\0", 50);
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

	size_t hashCounter;
	while((hashCounter = fread(buffer, 1, MAXDATASIZE, fp)) > 0) {  //Hash zbirke
		fileInfo->hashZbirke += hashCounter;
	}

	send(sockfd, &fileInfo, sizeof(struct myStruct), 0);
	send(sockfd, &imeZbirke, sizeof(imeZbirke), 0);

	// as long there is soething to read...
	size_t saveSize;
	while((saveSize = fread(buffer, 1, MAXDATASIZE, fp)) > 0) {  //buffer zbirke
		send(sockfd, buffer, saveSize, 0);
	}

	close(sockfd);
	
	return 0;
} 
