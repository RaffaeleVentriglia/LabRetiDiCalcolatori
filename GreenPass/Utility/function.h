#ifndef FUNCTION_H
#define FUNCTION_H

#include "wrapper.h"
#include "function.h"

#define CODELENGTH 16

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct GreenPass {
  char TesSan[CODELENGTH+1]; // codice fiscale
  int from; // provenienza del pacchetto
  int duration; // durata in mesi
  int status; // stato di validità del green pass
};

struct V_GreenPass {
  char TesSan[CODELENGTH+1]; // codice fiscale
  int from; // provenienza del pacchetto
  int status; // stato di validità del green pass
};

struct GreenPass *create_record(char *, int, int, int);
struct V_GreenPass *create_V_record(char *, int, int);
void search_record(struct GreenPass *, int);
int search_modify_record(struct GreenPass *, int);
void print_greenpass(struct GreenPass *);
off_t where_is(int, char *);
int see_status(char *, int);
int checkFrom(struct GreenPass *, int);

struct GreenPass *create_record(char *TS, int duration, int status, int from) {
    struct GreenPass *out = (struct GreenPass *)malloc(sizeof(struct GreenPass));
    strncpy(out->TesSan, TS, strlen(TS));
    out->duration = duration;
    out->status = status;
    out->from = from;
    return out;
}

struct V_GreenPass *create_V_record(char *TS, int from, int status) {
    struct V_GreenPass *out = (struct V_GreenPass *)malloc(sizeof(struct V_GreenPass));
    strncpy(out->TesSan, TS, strlen(TS));
    out->from = from;
    out->status = status;
    return out;
}

void search_record(struct GreenPass *gp, int fp) {
    char duration[2], status[2];
    if(where_is(fp, gp->TesSan) != -1) {
        // caso in cui il record esiste
        printf("Record esistente\n");
        return;
    } else {
        printf("Record non trovato, inizio la scrittura\n");
        write(fp, gp->TesSan, sizeof(gp->TesSan));
        sprintf(duration, "%d", gp->duration);
        sprintf(status, "%d", gp->status);
        write(fp, duration, 1);
        write(fp, status, 1);
        write(fp, "\n", 1);
        printf("Scrittura avvenuta con successo\n");
        return;
    }
}

int search_modify_record(struct GreenPass *gp, int fp) {
    off_t where = where_is(fp, gp->TesSan);
    char duration[2], status[2];
    printf("%ld\n", where);
    if(where > -1) {
        lseek(fp, where-17, SEEK_SET);
        write(fp, gp->TesSan, sizeof(gp->TesSan));
        sprintf(duration, "%d", gp->duration);
        sprintf(status, "%d", gp->status);
        write(fp, duration, 1);
        write(fp, status, 1);
        write(fp, "\n", 1);
        return 1;
    } else return 0;
}

void print_greenpass(struct GreenPass *gp) {
    if(gp == NULL) {
        fprintf(stderr, "Errore nella stampa del Green Pass\n");
        return;
    }
    printf("Dati Green Pass:\n");
    printf("Tessera Sanitaria: %s\n", gp->TesSan);
    printf("Durata: %d\n", gp->duration);
    printf("Provenienza: %d\n", gp->from);
}

off_t where_is(int fd, char *TS) {
    char buff[10];
    char tempTS[CODELENGTH];
    int row;
    off_t temp = 0;
    lseek(fd, 0, SEEK_SET);
    while((row = read(fd, buff, 10)) > 0) {
        temp += row;
        strncpy(tempTS, buff, sizeof(tempTS));
        tempTS[CODELENGTH+1] = '\0';
        if(strncmp(tempTS, TS, CODELENGTH) == 0)
            return temp;
        else {
            lseek(fd, 18, SEEK_CUR);
            temp += 18;
        }
    }
    return -1;
}

int see_status(char *TS, int fp) {
    char buff[2];
    off_t where = where_is(fp, TS);
    lseek(fp, (where - 2), SEEK_SET);
    if(where != -1)
        read(fp, buff, sizeof(buff));
    if(strchr(buff, '1'))
        return 1;
    else if(strchr(buff, '0'))
        return 0;
    else
        return 3;
}

int checkFrom(struct GreenPass* gp, int fd) {
    if(gp == NULL) {
        fprintf(stderr, "Green Pass vuoto, termino\n");
        return -1;
    }

    if(gp->from == 0) { // se il campo FROM vale 0, allora il pacchetto arriva dal centro vaccinale
        pthread_mutex_lock(&mutex);
        search_record(gp, fd);
        pthread_mutex_unlock(&mutex);
        return 2;
    } else if(gp->from == 1) {
        int status = 0;
        pthread_mutex_lock(&mutex);
        status = see_status(gp->TesSan, fd);
        pthread_mutex_unlock(&mutex);
        if(status > 1)
            return 1;
        else
            return status;
    } else if(gp->from == 2) {
        printf("Cerco Green Pass\n");
        int status = 3;
        pthread_mutex_lock(&mutex);
        status = search_modify_record(gp, fd);
        pthread_mutex_unlock(&mutex);
        if(status == 0)
            printf("Green Pass non presente\n");
        else if(status == 1)
            printf("Green Pass modificato\n");
        
        return 2;
    }

    return -1;
}

#endif