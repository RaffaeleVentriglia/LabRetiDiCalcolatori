#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>
#include <sys/select.h>

int Socket(int family, int type, int protocol);
void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t addrlen);
void IPconversion(int af, const char *src, struct in_addr *dst);
void Close(int fd);
void *GetCurrentDir(void);
void Chdir(const char *path);
void Remove(const char *pathname);
void Rename(const char *oldpath, const char *newpath);
ssize_t FullWrite(int fd, void *buf, size_t count);
ssize_t FullRead(int fd, void *buf, size_t count);

#endif