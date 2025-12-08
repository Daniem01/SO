#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "caldero.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>

int finish = 0;
struct memoria_compartida *caldero;
sem_t *sem_mutex, *sem_fill, *sem_empty;

void putServingsInPot(int servings)
{
	// Espera mientras el caldero no este vacio
	sem_wait(sem_empty);

	// Mutex
	sem_wait(sem_mutex);

	// Sirve y avisa de que hay comida
	caldero->raciones = servings;
	printf("Rellenando el caldero %d.\n", getpid());

	// Abrimos mutex
	sem_post(sem_mutex);

	for (int i = 0; i < M; i++)
	{
		sem_post(sem_fill);
	}
}

void cook(void)
{
	while (!finish)
	{
		putServingsInPot(M);
	}
}

void handler(int signo)
{
	finish = 1;
}

int main(int argc, char *argv[])
{
	int fd, t;
	void *m;

	// Obtenemos fichero para la memoria compartida. Se crea si no exite y modo lectura y escritura para todos.
	if ((fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) == -1)
	{
		perror("Error obteniendo el fichero para la memoria compartida.");
		return -1;
	}

	// Redimensionamos para que sea del tamaño del struct de memoria compartida
	if ((t = ftruncate(fd, sizeof(struct memoria_compartida))) == -1)
	{
		perror("Error redimensionando con ftruncate.");
		return -1;
	}

	// Mapeamos
	if ((m = mmap(NULL, sizeof(struct memoria_compartida), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		perror("Error mapeando usando mmap.");
		return -1;
	}
	caldero = (struct memoria_compartida *)m;
	caldero->raciones = 0;

	// Iniciamos los semaforos
	sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
	sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, 1);
	sem_fill = sem_open(SEM_FILL, O_CREAT, 0666, 0);

	// Cocinamos
	signal(SIGINT, handler);
	cook();

	// Limpiamos todo
	if ((sem_close(sem_mutex)) == -1)
	{
		perror("Error cerrando el semaforo.");
		return -1;
	}
	if ((sem_close(sem_empty)) == -1)
	{
		perror("Error cerrando el semaforo.");
		return -1;
	}
	if ((sem_close(sem_fill)) == -1)
	{
		perror("Error cerrando el semaforo.");
		return -1;
	}

	if ((sem_unlink(SEM_MUTEX)) == -1)
	{
		perror("Error usando unlink.");
		return -1;
	}
	if ((sem_unlink(SEM_FILL)) == -1)
	{
		perror("Error usando unlink.");
		return -1;
	}
	if ((sem_unlink(SEM_EMPTY)) == -1)
	{
		perror("Error usando unlink.");
		return -1;
	}

	if ((shm_unlink(SHM_NAME)) == -1)
	{
		perror("Error haciendo shm_unlink.");
		return -1;
	}

	return 0;
}
