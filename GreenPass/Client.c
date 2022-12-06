#include "function.h"

int main(int argc, char **argv) {
    int sock, vaccinato;
    struct sockaddr_in CentroVaccinale;
    sock = Socket(AF_INET, SOCK_STREAM, 0);
    CentroVaccinale.sin_family = AF_INET;
    CentroVaccinale.sin_port = htons(QUEUESIZE);
    IPconversion(AF_INET, argv[1], &CentroVaccinale.sin_addr);
    Connect(sock, (struct sockaddr *)&CentroVaccinale, sizeof(CentroVaccinale));
    vaccinato = 1;
    SendCode(stdin, sock);
    return 0;
}