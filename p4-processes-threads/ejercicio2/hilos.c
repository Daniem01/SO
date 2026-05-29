#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> // Libreria para compilar con hilos 

// Definicion de los parametros que piden
struct parametrosHilo {
	int numHilo;
	char prioridad;
};

// La funcion que ejecuta cada hilo recibiendo void* y devolviendo void*
void *thread_usuario(void *arg) {
	// Convertimos el puntero genérico (void*) a nuestra estructura
	struct parametrosHilo *p = (struct parametrosHilo *)arg;
	
    // Copiamos los datos a variables locales
	int num = p->numHilo;
	char pri = p->prioridad;

	// C) Liberamos la memoria que reservó el padre en el main, sino da fugas de memoria
	free(p);

    // Obtener el ID interno del hilo POSIX
	pthread_t t = pthread_self(); 

	printf("ID: %lu, POSICION: %d, PRIORIDAD: %c \n", (unsigned long)t, num, pri);

    // Se retorna NULL ya que no se necesita devolver resultados al hilo principal
	return NULL;
}

int main(int argc, char *argv[]) {
    // Si no me pasan el número de hilos por argumento, salgo con error 
    if (argc != 2) {
        printf("Error de uso. Ejecuta: %s <numero_de_hilos>\n", argv);
        return -1;
    }

	// Miramos cuantos hilos va a haber
	int hilos = atoi(argv[3]);
	
    // Array para guardar los identificadores de todos los hilos creados
	pthread_t arrayHilos[hilos];

	// CREACIÓN DE HILOS 
	for (int i = 0; i < hilos; i++) {
		
        // Reservamos memoria nueva para cada hilo.
		struct parametrosHilo *s = malloc(sizeof(struct parametrosHilo));

		// Rellenamos los datos del hilo actual
		s->numHilo = i;
		if (i % 2 == 0) {
			s->prioridad = 'P';
		} else {
			s->prioridad = 'N';
		}

		// Creamos el hilo  
        // arg1: Puntero a dónde guardar su ID 
        // arg2: Atributos (NULL por defecto)
        // arg3: Función a ejecutar (thread_usuario)
        // arg4: Parámetros de la función, en este caso el puntero con nuestra estructura
		if (pthread_create(&arrayHilos[i], NULL, thread_usuario, s) != 0) {
			perror("Error en la creacion del hilo.");
			return -1;
		}
	}

	// ESPERAR A LOS HILOS
    // El hiilo principal (main) debe esperar a que los hijos terminen. Si el main acaba antes los mata
	for (int j = 0; j < hilos; j++) {
        // pthread_join bloquea al main hasta que el hilo j termina
		if (pthread_join(arrayHilos[j], NULL) != 0) {
			perror("Error esperando hilo.");
			return -1;
		}
	}
    
	return 0;
}