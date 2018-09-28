#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#define MAX 10

typedef struct{
    sem_t esperando;
    sem_t consultando;
    sem_t ad;
    int done;
} semaforos;

void consulta(semaforos *sem_p){
    sem_post(&sem_p->ad);//comienza la consulta
    printf("Consultando...\n");
    sleep(2);
    sem_post(&sem_p->ad);//finaliza la consulta
    printf("Consulta terminada\n");
}

void *estudiante(semaforos *sem_p){
    if(sem_trywait(&sem_p->esperando)==0){//intento sentarme en una de las 3 sillas
        sem_wait(&sem_p->consultando);
        sem_post(&sem_p->esperando);
        consulta(sem_p);
        sem_post(&sem_p->consultando);
    }else{
        printf("Un estudiante se fue porque estaban las 3 sillas ocupadas.\n");
    }
    pthread_exit(EXIT_SUCCESS);
}

void *asistente(semaforos* sem_p){
    while(!sem_p->done){
        if(sem_trywait(&sem_p->ad)==0){//espero a que un estudiante entre a consultar

            sem_wait(&sem_p->ad);//espero a que finalice la consulta
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

int main()
{
    semaforos *sem_p = (semaforos*)malloc(sizeof(semaforos));
    sem_init(&sem_p->esperando,0,3);
    sem_init(&sem_p->consultando,0,1);
    sem_init(&sem_p->ad,0,0);
    sem_p->done = 0;
    pthread_t ad;
    pthread_create(&ad,NULL,asistente,sem_p);
    pthread_t threads[MAX];
    int i;
    for(i=0;i<MAX;i++){
        pthread_create(&threads[i],NULL,estudiante,sem_p);
        sleep(1);
    }
    for (i = 0; i < MAX; i++) {
		pthread_join(threads[i], NULL);
    }
    sem_p->done = 1;
    pthread_join(ad,NULL);
    sem_destroy(&sem_p->esperando);
    sem_destroy(&sem_p->consultando);
    sem_destroy(&sem_p->ad);
    free(sem_p);
    return 0;
}
