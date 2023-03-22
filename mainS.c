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

#define MAXDATASIZE 1024 //število prebranih zlogov

struct myStruct {
    int metapodatki;
    int dolzinaImeZbirke;
    int velikostZbirke;
    int hashZbirke;
    char imeZbirke[MAXDATASIZE];
    char buffer[MAXDATASIZE];
};

int main(int argc, char *argv[]) {
	struct stat file;
}