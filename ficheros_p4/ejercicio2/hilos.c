#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct parametrosHilo
{
	int numHilo;
	char prioridad;
};

void *thread_usuario(void *arg)
{
	// Variables locales y casting de la entrada
	struct parametrosHilo *p = (struct parametrosHilo *)arg;
	int num = p->numHilo;
	char pri = p->prioridad;

	// Liberamos memoria
	free(p);

	pthread_t t = pthread_self();

	printf("ID: %lu, POSICION: %d, PRIORIDAD: %c \n", (unsigned long)t, num, pri);

	return NULL;
}

int main(int argc, char *argv[])
{
	// Miramos cuantos hilos va a haber
	int hilos = atoi(argv[1]);
	pthread_t arrayHilos[hilos];

	// Bucle para crear los hilos
	for (int i = 0; i < hilos; i++)
	{
		// Creamos y reservamos en memoria
		struct parametrosHilo *s = malloc(sizeof(struct parametrosHilo));

		// Guardamos
		s->numHilo = i;
		if (i % 2 == 0)
		{
			s->prioridad = 'P';
		}
		else
		{
			s->prioridad = 'N';
		}

		// Creamos el hilo
		if (pthread_create(&arrayHilos[i], NULL, thread_usuario, s) != 0)
		{
			perror("Error en la creacion del hilo.");
			return -1;
		}
	}

	// Bucle para esperar a todos los hilos
	for (int j = 0; j < hilos; j++)
	{
		if (pthread_join(arrayHilos[j], NULL) != 0)
		{
			perror("Error esperando hilo.");
			return -1;
		}
	}
	return 0;
}
