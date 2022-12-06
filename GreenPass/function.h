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

// vari valori definiti per semplificazione
#define QUEUESIZE 1024
#define MAXPATH 4096
#define MAXLINE 256
#define CODELENGTH 21
#define MONTHS_FOR_NEXT_VACCINATION 5
#define MONTHS_IN_A_YEAR 12
#define DATELENGTH 11


/*
    varie enum che permettono di semplificare
    i valori di ritorno di ogni operazione 
    che viene eseguita
*/
enum CheckGreenPass {
    EXPIRED,
    OK,
    NOT_FOUND,
    DISABLED
};

enum UpdateGreenPass {
    NOT_UPDATED,
    UPDATED,
    ALREADY_ACTIVE,
    ALREADY_DISABLED
};

enum Code {
    CENTRO_VACCINALE,
    CLIENT_T,
    CLIENT_S
};

enum Vaccination {
    ALREADY_DONE,
    OK
};

// struttura dei pacchetti contenenti i dati dei green pass
typedef struct {
    char cardNumber[CODELENGTH];
    char validityDate[DATELENGTH];
    int value;
} GreenPass;

// struttura dei pacchetti aggiornati
typedef struct {
    char cardNumber[CODELENGTH];
    unsigned int value;
} GreenPassUpdate;


// insieme di tutte le funzioni
int Socket(int, int, int);
void Connect(int, const struct sockaddr *, socklen_t);
void Bind(int, const struct sockaddr *, socklen_t);
void Listen(int, int);
int Accept(int, struct sockaddr *, socklen_t *);
void IPconversion(int, const char *, struct in_addr *);
void Close(int);
void *GetCurrentDir(void);
void Chdir(const char *);
void Remove(const char *);
void Rename(const char *, const char *);
void SendCode(FILE *, int);
ssize_t FullWrite(int, void *, size_t);
ssize_t FullRead(int, void *, size_t);
int ConnectWithServerV(unsigned short int);
unsigned short int StringToPort(const char *, char **, int);
int greenPassCheck(const char *);
void checkTesseraSanitaria(char *);
char *greenPassValidity();
void updateValidity(const char *, int);
struct tm *dateToBDT(char *);


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

void SendCode(FILE *filein, int sock) {
    char buffer[CODELENGTH + 1];
    fflush(stdin);
    printf("Inserire codice fiscale: ");
    strcpy(buffer, "");
    if(fgets(buffer, CODELENGTH + 1, filein) == NULL) {
        Close(sock);
        return;
    } else {
        if((FullWrite(sock, (void *)buffer, (size_t)strlen(buffer))) < 0) {
            fprintf(stderr, "FullWrite error\n");
            exit(1);
        }
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


int ConnectWithServerV(unsigned short int serverPort) {
    int serverVsocket;
    struct sockaddr_in serverVaddress;
    serverVsocket = Socket(AF_INET, SOCK_STREAM, 0);
    serverVaddress.sin_family = AF_INET;
    serverVaddress.sin_port = htons(serverPort);
    IPconversion(AF_INET, "127.0.0.1", &serverVaddress.sin_addr);
    Connect(serverVsocket, (struct sockaddr *)&serverVaddress, sizeof(serverVaddress));
    return serverVsocket;
}

void checkTesseraSanitaria(char *cardNumber) {
    size_t length = strlen(cardNumber);
    if(length + 1 != CODELENGTH) {
        fprintf(stderr, "Tessera sanitaria errata\n");
        exit(1);
    }
}

unsigned short int stringToPort(const char *nptr, char **endptr, int base) {
    unsigned short int port;
    port = (unsigned short int) strtoul(nptr, endptr, base);
    if(port == 0 && (errno == EINVAL || errno == ERANGE)) {
        perror("strtoul");
        exit(1);
    }
    return port;
}

char *greenPassValidity() {
    char *expirationVaxDate;
    time_t systemTime = time(NULL);
    struct tm *expirationVaxDate = localtime((const time_t *) &systemTime);
    expirationVaxDate->tm_mday = 1;
    if(expirationVaxDate->tm_mon + 1 + MONTHS_FOR_NEXT_VACCINATION > MONTHS_IN_A_YEAR) {
        expirationVaxDate->tm_mon = (expirationVaxDate->tm_mon + 1 + MONTHS_FOR_NEXT_VACCINATION) % (MONTHS_IN_A_YEAR);
        expirationVaxDate->tm_year += 1;
    } else {
        expirationVaxDate->tm_mon = expirationVaxDate->tm_mon + MONTHS_FOR_NEXT_VACCINATION + 1;
    }
    expirationVaxDate = (char *)calloc(DATELENGTH, sizeof(char));
    if(expirationVaxDate == NULL) {
        perror("calloc");
        exit(1);
    }
    sprintf(expirationVaxDate, "%02d-%02d-%d", expirationVaxDate->tm_day, expirationVaxDate->tm_mon, 1900 + expirationVaxDate->tm_year);
    return expirationVaxDate;
}

struct tm *dateToBDT(char *date) {
    struct tm *res;
    char *token;
    const char *delimitatore = "-";
    int i = 0, dateInt[3];
    res = (struct tm *)calloc(1, sizeof(struct tm));
    if(res == NULL) {
        perror("calloc");
        exit(1);
    }
    token = strtok(date, delimitatore);
    while(token != NULL) {
        dateInt[i++] = atoi(token);
        token = strtok(NULL, delimitatore);
    }
    res->tm_mday = dateInt[0];
    res->tm_mon = dateInt[1];
    res->tm_year = dateInt[2];
    return res;
}

#endif