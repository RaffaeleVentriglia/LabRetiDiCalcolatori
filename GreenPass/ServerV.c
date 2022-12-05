#include "function.h"

int main(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    servaddr.sin_family = AF_INET;
}

