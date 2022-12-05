#include "function.h"

int Socket(int family, int type, int protocol) {
    int n;
    if((n = socket(family, type, protocol)) < 0) {
        perror("socket");
        exit(1);
    }
    return (n);
}