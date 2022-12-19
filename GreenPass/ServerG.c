#include "Utility/function.h"
#include "Utility/wrapper.h"

int main(int argc, char **argv) {
    int listFD, connFD;
    struct sockaddr_in ServerV, ServerG;
    pid_t pid;

    listFD = Socket(AF_INET, SOCK_STREAM, 0);
    ServerG.sin_family = AF_INET;
    ServerG.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerG.sin_port = htons(29);

    ServerV.sin_family = AF_INET;
    ServerV.sin_addr = ServerG.sin_addr;
    ServerV.sin_port = htons(31);

    Bind(listFD, (struct sockaddr *)&ServerG, sizeof(ServerG));
    printf("Bind effettuata\n");

    Listen(listFD, 1024);
    printf("Server in ascolto\n");

    for(;;) {
        connFD = Accept(listFD, (struct sockaddr *) NULL, NULL);
        pid = fork();

        if(pid < 0) {
            perror("fork");
            Close(connFD);
            exit(1);
        }

        if(pid == 0) {
            struct V_GreenPass tempGP;
            printf("Lettura del Green Pass dal socket\n");
            FullRead(connFD, &tempGP, sizeof(struct V_GreenPass));
            printf("Lettura del Green Pass avvenuta\n");
            int ServerVsock = Socket(AF_INET, SOCK_STREAM, 0);

            printf("Creo il Green Pass di risposta\n");
            if(tempGP.from == 1) {
                printf("La richiesta è stata ricevuta da ClientS\n");
                struct GreenPass* check_GP;
                char buffer[CODELENGTH];
                char ris[4];
                strncpy(buffer, tempGP.TesSan, sizeof(buffer));
                check_GP = create_record(buffer, 0, 0, 1);

                printf("Green Pass creato, avvio connessione con Server V\n");
                Connect(ServerVsock, (struct sockaddr *)&ServerV, sizeof(ServerV));
                printf("Connessione stabilita con il Server V\n");
                FullWrite(ServerVsock, check_GP, sizeof(struct GreenPass));
                printf("Green Pass inviato, in attesa di risposta\n");
                FullRead(ServerVsock, ris, 4);
                printf("Risposta ottenuta, comunico al Client S della validazione\n");
                Close(ServerVsock);
                FullWrite(connFD, ris, sizeof(ris));
                Close(connFD);
                exit(0);
            } else if(tempGP.from == 2) {
                int duration;
                if(tempGP.status == 0)
                    duration = 6;
                else
                    duration = 0;
                Connect(ServerVsock, (struct sockaddr *)&ServerV, sizeof(ServerV));
                struct GreenPass *GP;
                GP = create_record(tempGP.TesSan, duration, tempGP.status, 2);
                FullWrite(ServerVsock, GP, sizeof(struct GreenPass));
                printf("Green Pass inviato, chiusura della fork\n");
                Close(connFD);
                Close(ServerVsock);
                exit(0);
            }
        } else {
            close(0);
        }
    }
}
