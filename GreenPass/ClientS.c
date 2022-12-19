#include "Utility/function.h"
#include "Utility/wrapper.h"

int main(int argc, char **argv) {
    int ServerG;
    struct V_GreenPass *v_gp;
    char valid[4];
    struct sockaddr_in servadd;

    if(argc != 3) {
        fprintf(stderr, "Errore, l'input deve essere formato da: %s <IP-ADDRESS>, %s <TESSERA SANITARIA>\n", argv[1], argv[2]);
        exit(1);
    }

    ServerG = Socket(AF_INET, SOCK_STREAM, 0);
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

    printf("Connessione al Server G IP: %s\n", argv[1]);
    Connect(ServerG, (struct sockaddr *)&servadd, sizeof(servadd));
    printf("Connessione effettuata al server G, IP: %s\n", argv[1]);
    v_gp = create_V_record(argv[2], 1, 2);
    FullWrite(ServerG, v_gp, sizeof(struct V_GreenPass));
    printf("Green Pass inviato al Server G, in attesa di convalida...\n");
    FullRead(ServerG, valid, sizeof(valid));
    printf("Tessera Sanitaria: %s, Risposta: %s\n", argv[2], valid);
    printf("Arrivederci\n");
    return 0;
}