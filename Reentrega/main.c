#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX 100

typedef struct{
    sem_t sillas;//cuenta la cantidad de sillas vacias
    sem_t esperando;//cuenta la cantidad de alumnos esperando
    sem_t asistente;//mutex, solo un alumno por a la vez en la consulta
    sem_t consultando;
} semaforos;

void consulta(){
    printf("Estudiante: Consultando...\n");
    sleep(rand()%2);
    printf("Estudiante: Consulta terminada\n");
}

void *estudiante(void *sem_p){
    semaforos *sem = (semaforos*) sem_p;
    if(sem_trywait(&sem->sillas)==0){//intento sentarme en una de las 3 sillas
        printf("Estudiante: Me siento a esperar.\n");
        sem_post(&sem->esperando);//me siento a esperar
        sem_wait(&sem->asistente);//espero a que el asistente me llame
        sem_post(&sem->sillas);//entro a consulta, libero una silla
        consulta();//realizo la consulta
        sem_post(&sem->consultando);//termine la consulta
    }else{
        printf("Un estudiante se fue porque estaban las 3 sillas ocupadas.\n");
    }
    pthread_exit(EXIT_SUCCESS);
}

void *asistente(void* sem_p){
    semaforos *sem = (semaforos*) sem_p;
    while(1){
        if(sem_trywait(&sem->esperando)!=0){//Si no hay estudiantes esperando, me duermo
            printf("Asistente: Me dormi.\n");
            sem_wait(&sem->esperando);//Me despiertan
            printf("Asistente: Me despertaron.\n");
        }
        sem_post(&sem->asistente);//Voy a buscar un estudiante
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
    sem_init(&sem_p->asistente,0,0);
    sem_init(&sem_p->consultando,0,0);
    pthread_t ad;
    pthread_create(&ad,NULL,asistente,sem_p);
    pthread_t threads[MAX];
    int i;
    for(i=0;i<MAX;i++){
        pthread_create(&threads[i],NULL,estudiante,sem_p);
        sleep(rand()%2);
    }
    for (i = 0; i < MAX; i++) {//espero a que finalicen los hilos
		pthread_join(threads[i], NULL);
    }
    pthread_join(ad,NULL);
    //destruyo los semaforos creados
    sem_destroy(&sem_p->sillas);
    sem_destroy(&sem_p->esperando);
    sem_destroy(&sem_p->asistente);
    sem_destroy(&sem_p->consultando);
    free(sem_p);
    return 0;
}
