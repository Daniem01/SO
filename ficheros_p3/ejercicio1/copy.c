#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void copy(int fdo, int fdd)
{
	char buffer[512];
	ssize_t r = 0;

	// Mientras no se haya leido el final y no hay errores
	while ((r = read(fdo, buffer, 512)) > 0)
	{
		// Escribimos lo leido dentro del buffer al archivo destino
		ssize_t w = write(fdd, buffer, r);
		if (w == -1)
		{
			perror("Error escribiendo en el fichero destino");
			return;
		}
	}

	if (r == -1)
	{
		perror("Error leyendo el archivo origen.");
		return;
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Numero de argumentos erroneo. \n");
		return 1;
	}

	// Abrimos los ficheros
	int fo = open(argv[1], O_RDONLY);
	if (fo == -1)
	{
		perror("Error en la apertura del archivo origen.");
		return 1;
	}

	int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("Error abriendo el archivo destino.");
		return 1;
	}

	// Hacemos copy pasando los descriptores de fichero
	copy(fo, fd);

	// Cerramos los ficheros
	close(fd);
	close(fo);

	return 0;
}
