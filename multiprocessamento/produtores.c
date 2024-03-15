#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3

int buffer[BUFFER_SIZE];
int count = 0; // Contador de itens no buffer

// Semáforos para sincronização
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

// Função para produzir um item
void produce_item(int item) {
    printf("Produzindo item %d\n", item);
}

// Função para consumir um item
void consume_item(int item) {
    printf("Consumindo item %d\n", item);
}

// Função para inserir um item no buffer
void insert_item(int item) {
    buffer[count] = item;
    count++;
}

// Função para remover um item do buffer
int remove_item() {
    int item = buffer[count - 1];
    count--;
    return item;
}

// Função executada pelos produtores
void *producer(void *arg) {
    int item = 1;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&empty, &mutex);
        }
        produce_item(item);
        insert_item(item);
        item++;
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        sleep(1); // Simula produção
    }
    return NULL;
}

// Função executada pelos consumidores
void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            pthread_cond_wait(&full, &mutex);
        }
        int item = remove_item();
        consume_item(item);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        sleep(2); // Simula consumo
    }
    return NULL;
}

int main() {

    /*
        Threads: O código cria múltiplas threads para representar os produtores e consumidores. 
        As threads são unidades de execução independentes que podem ser executadas concorrentemente pelo kernel do Linux.
    */

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    // Criar threads para produtores
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    // Criar threads para consumidores
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // Aguardar a finalização das threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
