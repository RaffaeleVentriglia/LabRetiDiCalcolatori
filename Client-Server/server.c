#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buf[4096];
    time_t ticks;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(1);
    }
    servaddr.sin_family = AF_INET;
    /*
        INADDR_ANY viene utilizzato come indirizzo del server, quindi
        l'applicazione accetterà connessioni da qualsiasi indirizzo
        associato al server
    */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);
    /*
        la bind assegna l'indirizzo al socket, il cast è necessario
        perchè la funzione può essere usata con diversi tipi di socket
        e il terzo parametro è la dimensione in byte della struttura
    */
    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind");
        exit(2);
    }
    /*
        tramite listen si mette il socker in modalità di ascolto
        in attesa di nuove connessioni, il secondo parametro specifica
        quante connessioni possono essere in attesa di essere accettate
    */
    if(listen(listenfd, 1024) < 0) {
        perror("Listen");
        exit(3);
    }
    for(;;) {
        /*
            il secondo e il terzo argomento di accept servono ad
            identificare il client e possono essere NULL; il nuovo
            socket è associato alla nuova connessione, mentre il vecchio
            socket resta in ascolto
        */
        if((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) < 0) {
            perror("Accept");
            exit(4);
        }
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        if(write(connfd, buf, strlen(buf)) != strlen(buf)) {
            perror("Write");
            exit(5);
        }
        /*
            la close permette di termine una connessione TCP, e una
            volta invocato il descrittore del socket non è più
            utilizzabile dal processo per operazioni di lettura 
            o scrittura; il sottosistema di rete invia i dati in
            coda e successivamente chiude la connessione
        */
        close(connfd);
    }
}