#ifndef WRAPPER_H
#define WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>


// insieme di tutte le funzioni
int Socket(int, int, int);
void Connect(int, const struct sockaddr *, socklen_t);
void Bind(int, const struct sockaddr *, socklen_t);
void Listen(int, int);
int Accept(int, struct sockaddr *, socklen_t *);
void Close(int);
ssize_t FullWrite(int, void *, size_t);
ssize_t FullRead(int, void *, size_t);

#include "wrapper.h"

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

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int connfd;
    if((connfd = accept(sockfd, addr, addrlen)) < 0) {
        perror("accept");
        exit(1);
    }
    return connfd;
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

#endif