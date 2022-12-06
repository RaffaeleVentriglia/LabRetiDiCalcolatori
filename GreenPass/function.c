#include "function.h"

int Socket(int family, int type, int protocol) {
    int n;
    if((n = socket(family, type, protocol)) < 0) {
        perror("socket");
        exit(1);
    }
    return (n);
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if(connect(sockfd, addr, addrlen) < 0) {
        perror("connect");
        exit(1);
    }
}

void Close(int fd) {
    if(close(fd) < 0) {
        perror("close");
        exit(-1);
    }
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if(bind(sockfd, addr, addrlen) < 0) {
        perror(" bind");
        exit(1);
    }
}

void Listen(int sockfd, int backLog) {
    if(listen(sockfd, backLog) < 0) {
        perror("listen");
        exit(1);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t addrlen) {
    int connfd;
    if((connfd = accept(sockfd, addr, addrlen)) < 0) {
        perror("accept");
        exit(1);
    }
    return connfd;
}

void IPconversion(int af, const char *src, struct in_addr *dst) {
    if(inet_pton(AF_INET, src, dst) < 0) {
        fprintf(stderr, "inet_pton error: %s\n", src);
        exit(1);
    }
}

void *GetCurrentDir(void) {
    char *CurrWorkDir, *token, *directory;
    char buffer[MAXPATH + 1];
    size_t length;

    CurrWorkDir = getcwd(buffer, MAXPATH + 1);
    if(CurrWorkDir == NULL) {
        perror("getcwd");
        exit(1);
    }

    token = strrchr(CurrWorkDir, '/');
    if(token == NULL) {
        fprintf(stderr, "Directory mancante");
        exit(1);
    }

    length = strlen(token);
    directory = malloc(length);
    if(directory == NULL) {
        fprintf(stderr, "Errore nella malloc");
        exit(1);
    }
    memcpy(directory, token + 1, length);
    return directory;
}

void Chdir(const char *path) {
    if(strcmp((const char *)GetCurrentDir(), path) != 0) {
        if(chdir(path) == -1) {
            perror("chdir");
            exit(1);
        }
    }
}

void Remove(const char *pathname) {
    if(remove(pathname) == -1) {
        perror("remove");
        exit(1);
    }
}

void Rename(const char *oldpath, const char *newpath) {
    if(rename(oldpath, newpath) == -1) {
        perror("remove");
        exit(1);
    }
}

ssize_t FullWrite(int fd, void *buf, size_t count) {
    size_t nleft;
    ssize_t nwritten;
    nleft = count;
    while(nleft > 0) { // finchè non ci sono elementi a sinistra
        if((nwritten = write(fd, buf, count)) < 0) {
            if(errno == EINTR) // se interrotto tramite una system call
                continue; // ripeti il loop
            else {
                perror("write"); // altrimenti esci con l'errore
                exit(nwritten);
            }
        }
        nleft -= nwritten;
        buf += nwritten;
    }
    return nleft;
}

ssize_t FullRead(int fd, void *buf, size_t count) {
    size_t nleft;
    ssize_t nread;
    nleft = count;
    while(nleft > 0) { // finchè non ci sono elementi a sinistra
        if((nread = read(fd, buf, count)) < 0) {
            if(errno == EINTR)  // se interrotto tramite una system call
                continue; // ripeti il loop
            else {
                perror("read"); // altrimenti esci con l'errore
                exit(1);
            }
        } else if(nread == 0) // EOF
            break; // interrompi il loop
        nleft -= nread; // settiamo il left su read
        buf += nread; // settiamo il puntatore
    }
    buf = 0;
    return nleft;
}