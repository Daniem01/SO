#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

// Variables globales compartidas
int count; // Aforo actual
int vip;   // Contador de VIPs haciendo cola 

// PRIMITIVAS DE SINCRONIZACIÓN 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_normal = PTHREAD_COND_INITIALIZER; // Cola de normales
pthread_cond_t cond_vip = PTHREAD_COND_INITIALIZER;    // Cola de VIPs

// Struct de un cliente
struct client
{
	int id;
	int isvip;
};

// FUNCIONES DE ACCESO AL RECURSO
void enter_normal_client(int id)
{
	// Protocolo de entrada al monitor
	pthread_mutex_lock(&mutex);

	// Un normal no entra si la disco está llena 
    // Miramos si hay vips esperando en la puerta 
	while (count == CAPACITY || vip > 0)
	{
        // cond_wait suelta el mutex y duerme al hilo. Al despertar, recupera el mutex.
		pthread_cond_wait(&cond_normal, &mutex);
	}

	count++;
	printf("Cliente normal %d entra en la discoteca.\n", id);

	// Protocolo de salida
	pthread_mutex_unlock(&mutex);
}

void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex);
    
    // Sumamos 1 a los VIPs en cola antes del while
	vip++;

	// El VIP solo espera si la disco está llena. Le da igual si hay normales.
	while (count == CAPACITY)
	{
		pthread_cond_wait(&cond_vip, &mutex);
	}

	count++;
	vip--; // Ya hemos entrado entonces dejamos de hacer cola
	printf("Cliente vip %d entra en la discoteca.\n", id);

	pthread_mutex_unlock(&mutex);
}

void dance(int id, int isvip)
{
    // Esta función no lleva mutex porque no modifica variables globales.
    // Cada hilo puede bailar en paralelo.
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(&mutex);

	// Sale el cliente y libera una plaza
	count--;
	printf("Cliente %d (%s) sale de la discoteca.\n", id, VIPSTR(isvip));

	// DESPERTAR A OTROS HILOS
	// Si ha salido alguien hay un hueco libre. Miramos si hay vips con prioridad
	if (vip > 0)
	{
        // Si hay algún VIP esperando, despertamos al VIP
		pthread_cond_signal(&cond_vip);
	}
	else
	{
        // Solo si no hay VIP despertamos a un normal
		pthread_cond_signal(&cond_normal);
	}

	pthread_mutex_unlock(&mutex);
}

// FUNCIÓN PRINCIPAL DEL HILO
void *client(void *arg)
{
	struct client *c = arg;

	// Lógica de ruteo
	if (c->isvip)
	{
		enter_vip_client(c->id);
	}
	else
	{
		enter_normal_client(c->id);
	}

	dance(c->id, c->isvip);
	disco_exit(c->id, c->isvip);
    
    return NULL; 
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