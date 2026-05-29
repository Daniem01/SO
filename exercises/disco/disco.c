#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

// Prototipos de funciones
void enter_cleaning(void);
void exit_cleaning(void);

// Variables globales compartidas
int count; // Aforo actual
int vip;   // Contador de VIPs haciendo cola 
int clean_pending; // Variable global de limpieza
int last_clean; // Para saber hace cuanto hemos limpiado
int disco_close; // Controlamos el cierre 

// PRIMITIVAS DE SINCRONIZACIÓN 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_normal = PTHREAD_COND_INITIALIZER; // Cola de normales
pthread_cond_t cond_vip = PTHREAD_COND_INITIALIZER;    // Cola de VIPs
pthread_cond_t cond_clean = PTHREAD_COND_INITIALIZER;       // El limpiador

// Struct de un cliente
struct client
{
	int id;
	int isvip;
};

// Funcion del hilo limpiador
void* cleaning_thread(void* arg){
    while(1){
        enter_cleaning();
        
        // Si enter_cleaning determinó que la disco cerró Y no hay nada pendiente por limpiar
        if (disco_close == 1 && clean_pending == 0) {
            pthread_mutex_unlock(&mutex);
            break; 
        }
        
        // Limpiar la discoteca
        printf("Cleaning...\n");
        sleep(2);
        printf("Done!\n");
        
        exit_cleaning();
    }
    return NULL;
}

void enter_cleaning(){
    // Bloqueamos para acceder a las variables globales
    pthread_mutex_lock(&mutex);

    // Comprobamos si esta pendiente de limpiar
    // Comprobamos que la disco este abierta
    while(clean_pending == 0 && disco_close == 0){
        pthread_cond_wait(&cond_clean, &mutex);
    }

    // Si la disco ha cerrado nos vamos
    if(disco_close == 1 && clean_pending == 0){
        pthread_mutex_unlock(&mutex);
        return;
    }

    // Esperamos a que el resto que bailan se vayan
    while(count != 0){
        pthread_cond_wait(&cond_clean, &mutex);
    }

}

void exit_cleaning(){

    // Reseteamos limpieza
    last_clean = 0;
    clean_pending = 0;

    // Despertamos a los clientes
    if (vip > 0) {
        pthread_cond_signal(&cond_vip);
    } else {
        pthread_cond_signal(&cond_normal);
    }

    // Abrimos
    pthread_mutex_unlock(&mutex);
}

// FUNCIONES DE ACCESO AL RECURSO
void enter_normal_client(int id)
{
	// Protocolo de entrada al monitor
	pthread_mutex_lock(&mutex);

	// Un normal no entra si la disco está llena 
    // Miramos si hay vips esperando en la puerta 
	while (count == CAPACITY || vip > 0 || clean_pending == 1)
	{
        // cond_wait suelta el mutex y duerme al hilo. Al despertar, recupera el mutex.
		pthread_cond_wait(&cond_normal, &mutex);
	}

	count++;
    last_clean++;
	printf("Cliente normal %d entra en la discoteca.\n", id);

    // Si con este cliente llegamos a 3 toca limpiar
    if(last_clean >= 3){
        clean_pending = 1;
        pthread_cond_signal(&cond_clean);
    }
    else {
        // Si no hay que limpiar y queda sitio, invitamos al siguiente normal a entrar
        if (count < CAPACITY) {
            pthread_cond_signal(&cond_normal);
        }
    }

	// Protocolo de salida
	pthread_mutex_unlock(&mutex);
}

void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex);
    
    // Sumamos 1 a los VIPs en cola antes del while
	vip++;

	// El VIP solo espera si la disco está llena. Le da igual si hay normales.
	while (count == CAPACITY || clean_pending == 1)
	{
		pthread_cond_wait(&cond_vip, &mutex);
	}

	count++;
	vip--; // Ya hemos entrado entonces dejamos de hacer cola
    last_clean++;

    printf("Cliente vip %d entra en la discoteca.\n", id);

    // Si con este van 3 toca limpiar
    if(last_clean >= 3){
        clean_pending = 1;
        pthread_cond_signal(&cond_clean);
    }
    else {
        // Si no hay que limpiar y queda sitio, priorizamos despertar a otro VIP.
        // Si no hay VIPs, despertamos a un normal.
        if (count < CAPACITY) {
            if (vip > 0) {
                pthread_cond_signal(&cond_vip);
            } else {
                pthread_cond_signal(&cond_normal);
            }
        }
    }
	
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

    count--;
    printf("Cliente %d (%s) sale de la discoteca.\n", id, VIPSTR(isvip));

    // Si hay limpieza pendiente y somos el último en salir, despertamos al limpiador 
    if(clean_pending == 1){
        pthread_cond_broadcast(&cond_clean);
    } 
    // Si NO hay limpieza pendiente, entonces sí dejamos pasar a los clientes de la cola
    else if (clean_pending == 0) {
        if (vip > 0) {
            pthread_cond_signal(&cond_vip);
        } else {
            pthread_cond_signal(&cond_normal);
        }
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
    pthread_t limpiador;
    pthread_create(&limpiador, NULL, cleaning_thread, NULL);

    for (int i = 0; i < m; i++)
	{
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

    // Cerramos la discoteca y esperamos al limpiador
    pthread_mutex_lock(&mutex);
    disco_close = 1;
    pthread_cond_broadcast(&cond_clean); // Hay que avisar al limpiador
    pthread_mutex_unlock(&mutex);
    pthread_join(limpiador, NULL);

	// Cerramos el archivo
	if ((c = fclose(fd)) == EOF)
	{
		perror("Error cerrando el archivo.");
		return -1;
	}

	return 0;
}