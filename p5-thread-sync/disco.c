#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

// Variables globales para ver cuantos hay dentro y los vip que hay esperando.
int count;
int vip;

// Mutex y conditional
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_normal = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_vip = PTHREAD_COND_INITIALIZER;

// Struct de un cliente
struct client
{
	int id;
	int isvip;
};

void enter_normal_client(int id)
{
	// Bloqueamos otros hilos con mutex
	pthread_mutex_lock(&mutex);

	// Mientras no pueden entrar esperan
	while (count == CAPACITY || vip > 0)
	{
		pthread_cond_wait(&cond_normal, &mutex);
	}

	// Ya puede entrar
	count++;
	printf("Cliente normal %d entra en la discoteca.\n", id);

	// Liberamos hilo
	pthread_mutex_unlock(&mutex);
}

void enter_vip_client(int id)
{
	// Bloqueamos el resto de hilos
	pthread_mutex_lock(&mutex);
	vip++;

	// Mientras no pueden entrar esperan
	while (count == CAPACITY)
	{
		pthread_cond_wait(&cond_vip, &mutex);
	}

	// Puede entrar
	count++;
	vip--;
	printf("Cliente vip %d entra en la discoteca.\n", id);

	// Liberamos el hilo
	pthread_mutex_unlock(&mutex);
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	// Bloqueamos hilos
	pthread_mutex_lock(&mutex);

	// Sale cliente
	count--;
	printf("Cliente %d (%s) sale de la discoteca.\n", id, VIPSTR(isvip));

	// Si hay vips esperando se avisa a los vips sino a los normales
	if (vip > 0)
	{
		pthread_cond_signal(&cond_vip);
	}
	else
	{
		pthread_cond_signal(&cond_normal);
	}

	// Desbloqueamos mutex
	pthread_mutex_unlock(&mutex);
}

void *client(void *arg)
{
	struct client *c = arg;

	// Vemos quien mandamos entrar
	if (c->isvip)
	{
		enter_vip_client(c->id);
	}
	else
	{
		enter_normal_client(c->id);
	}

	// Baila y se va
	dance(c->id, c->isvip);
	disco_exit(c->id, c->isvip);
}

int main(int argc, char *argv[])
{
	int ret, m = 0, c;
	FILE *fd;

	// Vemos que nos hayan pasado 2 argumentos
	if (argc != 2)
	{
		fprintf(stderr, "Error: Debe ser ejecutable + archivo.");
		return -1;
	}

	// Abrimos el archivo
	if ((fd = fopen(argv[1], "r")) == NULL)
	{
		perror("Error abriendo el archivo");
		return -1;
	}

	// Leemos el archivo
	if ((ret = fscanf(fd, "%d", &m)) == EOF)
	{
		perror("Error escanenado el archivo.");
		return -1;
	}

	// Definimos el array de hilos y de clientes
	pthread_t hilos[m];
	struct client clientes[m];

	for (int i = 0; i < m; i++)
	{
		// Escaneamos para conseguir la info de cliente y crear su hilo
		int r;
		if ((r = fscanf(fd, "%d", &clientes[i].isvip)) == EOF)
		{
			perror("Error escaneando buscando info de cliente.");
			return -1;
		}
		clientes[i].id = i;

		pthread_create(&hilos[i], NULL, client, &clientes[i]);
	}

	// Bucle para que espere a que todos los hilos hayan cumplido su funcion
	for (int j = 0; j < m; j++)
	{
		pthread_join(hilos[j], NULL);
	}

	// Cerramos el archivo
	if ((c = fclose(fd)) == EOF)
	{
		perror("Error cerrando el archivo.");
		return -1;
	}

	return 0;
}
