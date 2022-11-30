#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    int sockfd, n;
    char recvline[1025];
    /*
        struct sockaddr_in {
            sa_family_t sin_family; // * AF_INET
            u_int16_t sin_port; // * porta in network order
            struct in_addr sin_addr;
        }
        struct in_addr {
            u_int32_t s_addr; // * indirizzo IP in network order
        }
    */
    struct sockaddr_in servaddr;
    if(argc != 2) {
        fprintf(stderr, "usage: %s <IP Address>\n", argv[0]);
        exit(1);
    }
    /*
        int socket(int famiglia, int tipo, int protocollo);
        La famiglia che utilizziamo è la AF_INET, ovvero
        il protocollo IPv4, e tutti i protocolli sono
        definiti all'interno di <sys/socket.h>
        Il tipo, SOCK_STREAM, sta ad indicarci che è un canale
        bidirezionale, sequenziale affidabile che opera su
        connessione; i dati vengono ricevuti e trasmessi
        come un unico flusso continuo.
        Per quanto riguarda il protocollo, normalmente ne esiste
        soltanto uno per supportare un determinato tipo di socket
        all'interno di una data famiglia di protocolli, e in tal caso
        può essere scritto semplicemente con il valore 0.
    */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error\n");
        exit(2);
    }
    servaddr.sin_family = AF_INET;
    /*
        h = host
        n = network
        l'istruzione seguente memorizza nel campo sin_port
        della struct servaddr l'intero 13 scritto in network order,
        13 è la porta su cui risponde il server che stiamo contattando
    */
    servaddr.sin_port = htons(13);
    /*
        la funzione inet_pton converte la stringa passata come
        secondo parametro in un indirizzo di rete scritto in
        network order e lo memorizza nella locazione di memoria
        puntata dal terzo parametro, quindi il programma deve
        specificare come argomento l'indirizzo IP del server
        a cui fare la richiesta
    */
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", argv[1]);
        exit(3);
    }
    /*
        la connect permette di connettere il socket all'indirizzo,
        il terzo argomento è la dimensione in byte della struttura;
        il cast è necessario in quanto la funzione può essere usata
        con diversi tipi di socket e quindi con diversi tipi di strutture
    */
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "connect error\n");
        exit(4);
    }
    while((n = read(sockfd, recvline, 1024)) > 0) {
        recvline[n] = 0;
        if(fputs(recvline, stdout) == EOF) {
            fprintf(stderr, "fputs error\n");
            exit(5);
        }
    }
    if(n < 0) {
        fprintf(stderr, "read error\n");
        exit(6);
    }
    exit(0);
}