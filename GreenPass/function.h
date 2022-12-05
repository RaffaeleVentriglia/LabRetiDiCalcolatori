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

#define MAXLINE 256
#define QUEUESIZE 1024
#define MAXPATH 4096

int Socket(int, int, int);
void Connect(int, const struct sockaddr *, socklen_t);
void Bind(int, const struct sockaddr *, socklen_t);
void Listen(int, int);
int Accept(int, struct sockaddr *, socklen_t);
void IPconversion(int, const char *, struct in_addr *);
void Close(int);
void *GetCurrentDir(void);
void Chdir(const char *);
void Remove(const char *);
void Rename(const char *, const char *);
ssize_t FullWrite(int, void *, size_t);
ssize_t FullRead(int, void *, size_t);

#endif