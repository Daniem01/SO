#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "caldero.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>

struct memoria_compartida *caldero;
sem_t *sem_mutex, *sem_fill, *sem_empty;

void getServingsFromPot(void)
{
	// Comen si hay comida
	sem_wait(sem_fill);

	// Mutex
	sem_wait(sem_mutex);

	// Comen
	caldero->raciones--;
	printf("Salvaje %d come. Quedan %d raciones.\n", getpid(), caldero->raciones);
	// Si esta vacio el caldero avisamos al cocinero
	if(caldero->raciones == 0){
		sem_post(sem_empty);
	}

	// Abrimos mutex
	sem_post(sem_mutex);
}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{
	// Comen NUMITER veces
	for(int i = 0; i < NUMITER; i++){
		getServingsFromPot();
		eat();
	}
}

int main(int argc, char *argv[])
{
	int fd;
	void *m;

	// Obtenemos fichero para la memoria compartida.
	if ((fd = shm_open(SHM_NAME, O_RDWR, 0)) == -1)
	{
		perror("Error obteniendo el fichero para la memoria compartida.");
		return -1;
	}

	// Mapeamos
	if ((m = mmap(NULL, sizeof(struct memoria_compartida), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		perror("Error mapeando usando mmap.");
		return -1;
	}
	caldero = (struct memoria_compartida *)m;

	// Creamos los semaforos
	sem_mutex = sem_open(SEM_MUTEX, 0);
	sem_empty = sem_open(SEM_EMPTY, 0);
	sem_fill = sem_open(SEM_FILL, 0);

	savages();

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

	return 0;
}
