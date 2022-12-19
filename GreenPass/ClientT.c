#include "Utility/function.h"
#include "Utility/wrapper.h"

int main(int argc, char **argv) {
    int serverGsock, valid;
    struct V_GreenPass *v_gp;
    struct sockaddr_in servadd;

    if(argc != 4) {
        fprintf(stderr, "Errore, l'input deve essere formato da: %s <IP-ADDRESS>, %s <TESSERA SANITARIA>, %s <VALIDATE>\n", argv[1], argv[2], argv[3]);
        exit(1);
    }

    serverGsock = Socket(AF_INET, SOCK_STREAM, 0);
    servadd.sin_family = AF_INET;
    servadd.sin_port = htons(29);

    if(inet_pton(AF_INET, argv[1], &servadd.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error\n");
        exit(1);
    }
    
    if(strlen(argv[2]) <= 0 || strlen(argv[2]) > CODELENGTH) {
        fprintf(stderr, "Errore nel size della tessera sanitaria\n");
        exit(1);
    }

    valid = atoi(argv[3]);
    if(valid < 0 || valid > 1) {
        fprintf(stderr, "Errore nella lettura della validazione, deve essere 0 (valido) oppure 1 (non valido)\n");
        exit(1);
    }

    printf("Connessione al server G, IP: %s\n", argv[1]);
    Connect(serverGsock, (struct sockaddr *)&servadd, sizeof(servadd));
    printf("Connessione effettuata al server G\n");
    v_gp = create_V_record(argv[2], 2, valid);
    FullWrite(serverGsock, v_gp, sizeof(struct V_GreenPass));
    printf("Green Pass inviato al server G, arrivederci\n");
    exit(0);
}
