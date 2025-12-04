#define _POSIX_C_SOURCE 200809L // Si no activo con esto no me va en mi linux nativo sigaction
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

/*programa que temporiza la ejecución de un proceso hijo */

// Variables globales de alarma
unsigned int alarma = 5;
pid_t PID;

// Funcion que controla al hijo, aunque no usamos signal hay que declararlo ya que sino mo se puede igualar a sa_handler
void tratar_alarma(int signal)
{
	// Solo imprimimos si realmente matamos a alguien
	if (PID > 0)
	{
		printf("\nTiempo excedido. Matando al hijo (PID %d)\n", PID);
		kill(PID, SIGKILL);
	}
}

int main(int argc, char **argv)
{
	int status;

	// Creamos el sigaction
	struct sigaction sa;
	// Asignamos la funcion
	sa.sa_handler = tratar_alarma;
	//  Limpiamos la configuracion
	sigemptyset(&sa.sa_mask);

	sigaction(SIGALRM, &sa, NULL);

	// Hacemos fork
	PID = fork();

	// Hijo
	if (PID == 0)
	{
		if (execvp(argv[1], &argv[1]) == -1)
		{
			perror("Error haciendo execvp.");
			return -1;
		}
	}
	// Padre
	else if (PID > 0)
	{
		alarm(alarma);
		wait(&status);

		alarm(0);

		// Comprobamos como termino el hijo
		if (WIFEXITED(status))
		{
			printf("El hijo finalizó normalmente (Exit code: %d)\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			printf("El hijo termino por una señal. (Signal code: %d) \n", WTERMSIG(status));
		}
	}
	// Otro = error
	else
	{
		perror("Error haciendo el fork.");
		return -1;
	}

	return 0;
}
