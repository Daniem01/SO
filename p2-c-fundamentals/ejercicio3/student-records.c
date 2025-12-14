#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"

extern char *optarg;

/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

int print_text_file(char *path)
{
	SimpleRecord t;
	FILE *fd;
	int s, c;

	// Abrimos el archivo con el path que nos pasan
	if ((fd = fopen(path, "r")) == NULL)
	{
		perror("Error abriendo el archivo con fopen.");
		return -1;
	}

	// Hacemos el fscanf y los mostramos por pantalla
	while ((s = fscanf(fd, "%d %lf %s", &t.id, &t.value, t.label)) == 3)
	{
		printf("Id: %d, Valor: %lf, Etiqueta: %s \n", t.id, t.value, t.label);
	}

	// Cerramos
	if ((c = fclose(fd)) == EOF)
	{
		perror("Error cerrando el fichero.");
		return -1;
	}

	return 0;
}

int print_binary_file(char *path)
{
	FILE *fd;
	int r, c;
	SimpleRecord sr;

	// Abrimos el archivo con el path que nos pasan
	if ((fd = fopen(path, "rb")) == NULL)
	{
		perror("Error abriendo el archivo con fopen.");
		return -1;
	}

	// Leemos lo que contiene el archivo
	while ((r = fread(&sr, sizeof(SimpleRecord), 1, fd)) == 1)
	{
		printf("Id: %d, Valor: %lf, Etiqueta: %s \n", sr.id, sr.value, sr.label);
	}

	// Cerramos
	if ((c = fclose(fd)) == EOF)
	{
		perror("Error cerrando el fichero.");
		return -1;
	}

	return 0;
}

int write_binary_file(char *input_file, char *output_file)
{
	FILE *fo, *fi;
	SimpleRecord t;
	int s, w, c1, c2;

	// Abrimos el archivo con el path que nos pasan
	if ((fi = fopen(input_file, "r")) == NULL)
	{
		perror("Error abriendo el archivo input.");
		return -1;
	}
	if ((fo = fopen(output_file, "wb")) == NULL)
	{
		perror("Error abriendo el archivo output.");
		return -1;
	}

	// Usamos fsacanf para la entrada y vamos escribiendo en la salida
	while ((s = fscanf(fi, "%d %lf %s", &t.id, &t.value, t.label)) == 3)
	{
		if ((w = fwrite(&t, sizeof(SimpleRecord), 1, fo)) != 1)
		{
			perror("Error escribiendo en el output_file.");
			return -1;
		}
	}

	// Cerramos los archivos
	if ((c1 = fclose(fi)) == EOF)
	{
		perror("Error cerrando el fichero.");
		return -1;
	}
	if ((c2 = fclose(fo)) == EOF)
	{
		perror("Error cerrando el fichero.");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:pbo:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;
		case 'o':
			options.output_file = optarg;
			options.action = WRITE_BINARY_ACT;
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
