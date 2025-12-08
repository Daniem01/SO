#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */

char *loadstr(FILE *file)
{
	// Variable para el tamaño de la cadena y el EOF
	size_t len = 0;
	int e;
	long start = ftell(file);

	while ((e = fgetc(file)) != '\0' && e != EOF)
	{
		len++;
	}

	// Si se llega al final y no hay nada devolver NULL
	if (e == EOF && len == 0)
	{
		return NULL;
	}

	// Volvemos al principio
	fseek(file, start, SEEK_SET);

	// Reservamos memoria
	char *str = malloc(len + 1);
	if (!str)
	{
		return NULL;
	}

	// Leemos cadena
	fread(str, sizeof(char), len + 1, file);

	return str;
}

int main(int argc, char *argv[])
{
	// Comprobamos que los argumentos sean el numero correcto
	if (argc != 2)
	{
		perror("Error al intoducir los argumentos \n");
		return 1;
	}

	// Abrimos el archivo
	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		perror("No se ha podido abrir el archivo");
		return 1;
	}

	// Pintamos
	char *cadena = NULL;
	while ((cadena = loadstr(input)) != NULL)
	{
		printf("%s \n", cadena);
		free(cadena);
	}

	fclose(input);

	return 0;
}
