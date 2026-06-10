#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MAX_SBUFFER_SIZE 4
#define MAX_LINE_LENGTH 256

char* shared_buffer[MAX_SBUFFER_SIZE]; 
FILE *entrada;
FILE *salida;

// Índices para gestionar el buffer circular y contador de elementos
int index_prod = 0;
int index_cons = 0;
int count = 0;

// Mecanismos de sincronización POSIX 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_lleno  = PTHREAD_COND_INITIALIZER; // Para bloquear al productor si se llena
pthread_cond_t cond_vacio  = PTHREAD_COND_INITIALIZER; // Para bloquear al consumidor si se vacía

void* productor(void* arg) {
    char buffer[MAX_LINE_LENGTH];

    while(fgets(buffer, MAX_LINE_LENGTH, entrada) != NULL){
        char *copia = strdup(buffer);

        pthread_mutex_lock(&mutex);
        
        // Esperamos hasta que haya hueco 
        while (count == MAX_SBUFFER_SIZE) {
            pthread_cond_wait(&cond_lleno, &mutex);
        }
        
        shared_buffer[index_prod] = copia;

        // Avanzamos el índice de forma circular para la próxima vuelta 
        index_prod = (index_prod + 1) % MAX_SBUFFER_SIZE;

        // Sumamos 1 al contador porque ahora hay un elemento más guardado
        count++;

        // Despertamos al consumidor
        pthread_cond_signal(&cond_vacio);
    
        pthread_mutex_unlock(&mutex);
    }

    // Mandamos mensaje de fin
    pthread_mutex_lock(&mutex);

    while (count == MAX_SBUFFER_SIZE) {
            pthread_cond_wait(&cond_lleno, &mutex);
    }

    shared_buffer[index_prod] = NULL;
    index_prod = (index_prod + 1) % MAX_SBUFFER_SIZE;
    count++;

    pthread_cond_signal(&cond_vacio);
    pthread_mutex_unlock(&mutex);
        
    return NULL;
}

void* consumidor(void *arg){
    char *linea;
    
    while(1) { 
        pthread_mutex_lock(&mutex);

        while(count == 0){
            pthread_cond_wait(&cond_vacio, &mutex);
        }

        // Extracción segura del puntero
        linea = shared_buffer[index_cons];

        index_cons = (index_cons + 1) % MAX_SBUFFER_SIZE;
        count--;
        
        // Avisamos al productor de que hay un hueco libre
        pthread_cond_signal(&cond_lleno);
        pthread_mutex_unlock(&mutex);

        // Comprobacion para salir
        if (linea == NULL) {
            break; 
        }

        // Escribimos y liberamos cada línea 
        fputs(linea, salida);
        free(linea);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int opt;
    // Por defecto, apuntamos a la entrada y salida estandar
    entrada = stdin;
    salida = stdout;

    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
            case 'i':
                // Abrir el fichero en lectura
                entrada = fopen(optarg, "r");
                if(entrada == NULL){
                    perror("Error abriendo el archivo origen");
                    exit(1);
                }
                break;
            case 'o':
                // Abrimos el fichero en escritura
                salida = fopen(optarg, "w");
                if(salida == NULL){
                    perror("Error abriendo el archivo destino");
                    exit(1);
                }

                break;
            case 'h':
                // Mostrar ayuda
                printf("Uso: %s [-i fichero_entrada] [-o fichero_salida] [-h]\n", argv[0]);
                return 0;
            default:
                fprintf(stderr, "Uso: %s [-i fichero_entrada] [-o fichero_salida] [-h]\n", argv[0]);
                return 1;
        }
    }

    // Identificadores de los hilos 
    pthread_t th_prod, th_cons;

    // Creación de los hilos Productor y Consumidor
    if (pthread_create(&th_prod, NULL, productor, NULL) != 0) {
        perror("Error al crear el hilo productor");
        exit(1);
    }
    if (pthread_create(&th_cons, NULL, consumidor, NULL) != 0) {
        perror("Error al crear el hilo consumidor");
        exit(1);
    }

    // Espera obligatoria a que terminen ambos hilos antes de cerrar el programa [cite: 65]
    pthread_join(th_prod, NULL);
    pthread_join(th_cons, NULL);

    // Al final del programa, cerramos los ficheros si no son stdin/stdout
    if (entrada != stdin) fclose(entrada);
    if (salida != stdout) fclose(salida);

    return 0;
}