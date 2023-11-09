#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <pthread.h>

struct mutex {
    CRITICAL_SECTION lock;
};

struct condvar {
    CONDITION_VARIABLE condition;
};

void mutex_init(struct mutex *m) {
    InitializeCriticalSection(&m->lock);
}

void mutex_lock(struct mutex *m) {
    EnterCriticalSection(&m->lock);
}

void mutex_unlock(struct mutex *m) {
    LeaveCriticalSection(&m->lock);
}

void condvar_init(struct condvar *c) {
    InitializeConditionVariable(&c->condition);
}

void condvar_wait(struct condvar *c, struct mutex *m) {
    SleepConditionVariableCS(&c->condition, &m->lock, INFINITE);
}

void condvar_signal(struct condvar *c) {
    WakeConditionVariable(&c->condition);
}

void condvar_broadcast(struct condvar *c) {
    WakeAllConditionVariable(&c->condition);
}

#define MAX_THREADS 5

struct shared_data {
    struct mutex mutex;
    struct condvar cond;
    int shared_variable;
};

void *thread_function(void *arg) {
    struct shared_data *data = (struct shared_data *)arg;

    for (int i = 0; i < 10; ++i) {
        mutex_lock(&data->mutex);
        data->shared_variable++; // Incrementa a variável compartilhada
        printf("Thread %ld - Valor compartilhado: %d\n", GetCurrentThreadId(), data->shared_variable);
        mutex_unlock(&data->mutex);

        // Espera por um curto período de tempo antes de incrementar novamente
        Sleep(100);
    }
    return NULL;
}

int main() {
    pthread_t threads[MAX_THREADS];
    struct shared_data data;

    mutex_init(&data.mutex);
    condvar_init(&data.cond);
    data.shared_variable = 0;

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_function, &data);
    }

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
