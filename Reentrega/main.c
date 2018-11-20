#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX 10

typedef struct{
    sem_t sillas;//cuenta la cantidad de sillas vacias
    sem_t esperando;//cuenta la cantidad de alumnos esperando
    sem_t asistente;//mutex, solo un alumno por a la vez en la consulta
    sem_t consultando;
    int done;
} semaforos;

void consulta(semaforos *sem_p){
    printf("Estudiante: Consultando...\n");
    sleep(2);
    printf("Estudiante: Consulta terminada\n");
    sem_post(&sem_p->consultando);//termine la consulta
}

void *estudiante(void *sem_p){
    semaforos *sem = (semaforos*) sem_p;
    if(sem_trywait(&sem->sillas)==0){//intento sentarme en una de las 3 sillas
        sem_post(&sem->esperando);//me siento a esperar
        sem_wait(&sem->asistente);//espero a que el asistente este libre
        sem_post(&sem->sillas);//entro a consulta, libero una silla
        consulta(sem);//realizo la consulta
        sem_post(&sem->asistente);//libero al asistente
    }else{
        //printf("Un estudiante se fue porque estaban las 3 sillas ocupadas.\n");
    }
    pthread_exit(EXIT_SUCCESS);
}

void *asistente(void* sem_p){
    semaforos *sem = (semaforos*) sem_p;
    while(!sem->done){
        sem_wait(&sem->esperando);//si no hay alumnos duermo
        //me despertaron
        printf("Asistente: Dando consulta\n");
        sem_wait(&sem->consultando);//espero a que termine la consulta
        printf("Asistente: Consulta terminada\n");
    }
    pthread_exit(EXIT_SUCCESS);
}

int main()
{
    semaforos *sem_p = (semaforos*)malloc(sizeof(semaforos));
    sem_init(&sem_p->sillas,0,3);
    sem_init(&sem_p->esperando,0,0);
    sem_init(&sem_p->asistente,0,1);
    sem_init(&sem_p->consultando,0,0);
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
    sem_destroy(&sem_p->sillas);
    sem_destroy(&sem_p->esperando);
    sem_destroy(&sem_p->asistente);
    sem_destroy(&sem_p->consultando);
    free(sem_p);
    return 0;
}
