#include "Utility/function.h"
#include "Utility/wrapper.h"

int main(int argc, char **argv) {
    int CV_socket; // file descriptor per la socker del centro vaccinale
    char *buff; // buffer per operazioni di lettura/scrittura
    struct sockaddr_in serv_addr; // struct della socket del centro vaccinale

    // Controllo argomenti passati da linea di comando
    if(argc != 3) {
        fprintf(stderr, "Errore, l'input deve essere formato da: %s <IP-ADDRESS>, %s <TESSERA SANITARIA>\n", argv[1], argv[2]);
        exit(1);
    }

    // Imposto la socket per il Centro Vaccinale
    CV_socket = Socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(28);
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Errore inet_pton\n");
        exit(1);
    }

    // Controllo sulla lunghezza della tessera sanitaria
    if(strlen(argv[2]) <= 0 || strlen(argv[2]) > CODELENGTH) {
        fprintf(stderr, "Size della tessera sanitaria non compatibile\n");
        exit(1);
    }
    buff = argv[2];

    // Connessione al Centro Vaccinale
    printf("Connessione al Centro Vaccinale: %s\n", argv[1]);
    Connect(CV_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connessione al Centro Vaccinale effettuata\n");

    // Invio al Centro Vaccinale della tessera sanitaria
    FullWrite(CV_socket, buff, strlen(buff));
    printf("Tessera Sanitaria inviata al Centro Vaccinale\n");
    exit(0);
}