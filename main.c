#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX 100

int esperando;

typedef struct{
    sem_t sem;
} semaforo;

void estudiante(void *sem_p){
    if(esperando==3){
        pthread_exit(EXIT_SUCCESS);
    }
    esperando++;
    sem_t sema;
    sema = ((semaforo *)sem_p)->sem;
    wait(&sema);
    esperando--;
    consulta();
    sem_post(&sema);
    pthread_exit(EXIT_SUCCESS);


}
void consulta(){

}



int main()
{
    esperando = 0;
    pthread_t threads[MAX];
    semaforo *sem_p = (semaforo*)malloc(sizeof(semaforo));
    sem_init(&sem_p->sem,0,1);
    int i;
    for(i=0;i<MAX;i++){
    pthread_create(&threads[i],NULL,estudiante,(void *)sem_p);



    }







    return 0;
}
