#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char *argv[])
{
	// Comprobar argumentos
	if (argc < 3)
	{
		fprintf(stderr, "Numero de argumentos incorrecto. \n", argv[0]);
		return 1;
	}

	// Leemos el nombre del archivo de texto
	char *filename = argv[1];
	FILE *outputFile;

	// Abrimos el archivo
	outputFile = fopen(filename, "wb");
	if (outputFile == NULL)
	{
		perror("Fallo abriendo el archivo \n");
		return 1;
	}

	// Recorremos un bucle para coger todos los argumentos que nos han pasado
	for (int i = 2; i < argc; i++)
	{
		char *town = argv[i];
		size_t size = strlen(town);

		// Escribimos el pais en el fichero
		size_t written = fwrite(town, sizeof(char), size + 1, outputFile);
		if (written != size + 1)
		{
			perror("Error escribiendo en el fichero \n");
			fclose(outputFile);
			return 1;
		}
	}

	// Cerramos el archivo una vez manipulado
	fclose(outputFile);

	printf("Proceso de escritura terminado con exito \n");

	return 0;
}
