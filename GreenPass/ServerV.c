#include "Utility/function.h"
#include "Utility/wrapper.h"

int main(int argc, char **argv) {
    int listFD, connFD, fd;
    struct sockaddr_in server_v;
    socklen_t length;
    pid_t pid;
    char *res;

    printf("Apertura del file GreenPass.txt\n");
    if((fd = open("GreenPass.txt", O_RDWR || O_CREAT, 0666)) < 1) {
        fprintf(stderr, "Errore nell'apertura del file\n");
        exit(1);
    }

    listFD = Socket(AF_INET, SOCK_STREAM, 0);

    // struttura socket per il server V
    server_v.sin_family = AF_INET;
    server_v.sin_addr.s_addr = htonl(INADDR_ANY);
    server_v.sin_port = htons(31);

    Bind(listFD, (struct sockaddr *)&server_v, sizeof(server_v));
    printf("Bind effettuata con successo\n");

    Listen(listFD, 1024);
    printf("Server in ascolto\n");

    for(;;) {
        connFD = Accept(listFD, (struct sockaddr *) NULL, NULL);
        pid = fork();
        if(pid < 0) {
            perror("Fork");
            Close(connFD);
            exit(1);
        }
        if(pid == 0) {
            struct GreenPass gp;
            printf("Leggo Green Pass dal Socket\n");
            FullRead(connFD, &gp, sizeof(struct GreenPass));
            printf("Lettura eseguita con successo\n");

            int where = checkFrom(&gp, fd);

            if(where == 0) {
                printf("Validazione del Green Pass effettuata con successo\n");
                res = "SI\0";
                printf("%s\n", res);
                FullWrite(connFD, res, strlen(res));
                printf("Report inviato\n");
            } else if(where == 1) {
                printf("Validazione del Green Pass effettuata con successo\n");
                res = "NO\0";
                printf("%s\n", res);
                FullWrite(connFD, res, strlen(res));
                printf("Report inviato\n");
            } else if(where == 2) {

            } else {
                fprintf(stderr, "Errore, la richiesta non può essere elaborata\n");
            }
            printf("Chiusura in corso\n");
            Close(connFD);
            exit(0);
        } else {
            Close(0);
        }
    }
}

