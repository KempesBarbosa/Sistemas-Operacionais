#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>

// Estrutura do Mutex
struct mutex {
    atomic_uint v;
};

// Funções Mutex
void mutex_init(struct mutex *m)
{
    m->v = 0;
}

void mutex_travar(struct mutex *m)
{
    for (;;) {
        uint32_t v = 0;
        if (atomic_compare_exchange_strong(&m->v, &v, 1)) {
            break;
        }
    }
}

void mutex_destravar(struct mutex *m)
{
    atomic_store(&m->v, 0);
}

// Estrutura do Semáforo
struct sem {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    unsigned valor;
};

// Funções para o Semáforo
void sem_init(struct sem *s, unsigned val) {
    pthread_mutex_init(&(s->mutex), NULL);
    pthread_cond_init(&(s->condition), NULL);
    s->valor = val;
}

void sem_inc(struct sem *s) {
    mutex_travar((struct mutex *)s); // Utiliza um mutex para garantir atomicidade
    s->valor++;
    pthread_cond_signal(&(s->condition)); // Acorda uma thread que esteja esperando
    mutex_destravar((struct mutex *)s);
}

void sem_dec(struct sem *s) {
    mutex_travar((struct mutex *)s); // Utiliza um mutex para garantir atomicidade
    while (s->valor == 0) {
        // Aguarda até que o semáforo seja incrementado por outra thread
        pthread_cond_wait(&(s->condition), &(s->mutex));
    }
    s->valor--;
    mutex_destravar((struct mutex *)s);
}

// Declaração de instância 
struct mutex axMutex;
struct sem axSemaforo;

void *thread_function(void *arg) {
    int thread_id = *((int *)arg);

    // Aguarda para entrar na seção crítica usando semáforo
    printf("Thread %d: Aguardando para entrar na seção crítica.\n", thread_id);

    // Entra na seção crítica
    sem_dec(&axSemaforo);
    printf("Thread %d: Entrou na seção crítica.\n", thread_id);

    // Sai da seção crítica
    axSemaforo.valor++;  
    printf("Thread %d: Saindo da seção crítica. \nValor atual do semáforo: %u\n", thread_id, axSemaforo.valor);
    sem_inc(&axSemaforo);

    return NULL;
}

int main() {
    int i, num_threads = 3;
    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    // Inicializa o mutex e o semáforo
    mutex_init(&axMutex);
    sem_init(&axSemaforo, 1); // Inicia com valor 1 para permitir a entrada na seção crítica

    // Cria threads
    for (i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }

    // Aguarda o término das threads
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
