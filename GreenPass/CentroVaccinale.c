#include "Utility/function.h"
#include "Utility/wrapper.h"

int main(int argc, char **argv) {
    int listFD, connFD;
    struct sockaddr_in CV_server, V_server;
    char buff[CODELENGTH];
    pid_t pid;
    listFD = Socket(AF_INET, SOCK_STREAM, 0);

    // struttura socket del Centro Vaccinale
    CV_server.sin_family = AF_INET;
    CV_server.sin_addr.s_addr = htonl(INADDR_ANY);
    CV_server.sin_port = htons(28);

    // struttura socket del ServerV
    V_server.sin_family = AF_INET;
    V_server.sin_addr = CV_server.sin_addr;
    V_server.sin_port = htons(31);

    Bind(listFD, (struct sockaddr *)&CV_server, sizeof(CV_server));
    printf("Bind effettuata\n");

    Listen(listFD, 1024);
    printf("Server in ascolto\n");

    for(;;) {
        connFD = Accept(listFD, (struct sockaddr *) NULL, NULL);
        pid = fork();
        if(pid < 0) {
            perror("Fork");
            Close(connFD);
            exit(-1);
        }

        if(pid == 0) {
            printf("Lettura della tessera sanitaria\n");
            FullRead(connFD, buff, sizeof(buff));
            printf("Lettura avvenuta con successo\n");
            Close(connFD);

            int ServerV_sock = Socket(AF_INET, SOCK_STREAM, 0);
            Connect(ServerV_sock, (struct sockaddr *)&V_server, sizeof(V_server));
            printf("Connessione avvenuta con successo con il Server V\n");

            struct GreenPass *newGP;
            newGP = create_record(buff, 6, 0, 0);
            printf("Green Pass creato con successo, invio al Server V\n");
            print_greenpass(newGP);
            FullWrite(ServerV_sock, newGP, sizeof(struct GreenPass));
            printf("Green Pass inviato con successo\n");
            Close(ServerV_sock);
            exit(0);
        } else {
            close(0);
        }
    }
}
