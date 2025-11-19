#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // Para struct stat, lstat y macros S_ISREG, S_ISLNK, etc.
#include <unistd.h>	  // Para readlink y symlink
#include <fcntl.h>

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

void copy_regular(char *orig, char *dest)
{
	// Abrimos los ficheros
	int fo = open(orig, O_RDONLY);
	if (fo == -1)
	{
		perror("Error en la apertura del archivo origen.");
		return;
	}

	int fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("Error abriendo el archivo destino.");
		return;
	}

	// Hacemos copy pasando los descriptores de fichero
	copy(fo, fd);

	// Cerramos los ficheros
	close(fd);
	close(fo);
}

void copy_link(char *orig, char *dest)
{
	struct stat s;
	int p;
	size_t size = 0;
	ssize_t r = 0;

	// Hacemos un lstat para sacar de la estructua el tamaño del fichero
	p = lstat(orig, &s);
	if(p == -1){
		perror("Error haciendo lstat en la copia.");
		return;
	}

	size = s.st_size;

	// Reservamos memoria
	char *buffer = malloc(size + 1);
	if(!buffer){
		fprintf(stderr, "Error reservando memoria");
		return;
	}

	// Hacemos el read_link para sacar la ruta al fichero
	r = readlink(orig, buffer, size);
	// Si falla cerramos y sino al final de lbuffer metemos \0, NO LO HACE AUTOMATICAMENTE
	if (r == -1){
		perror("Error leyendo la ruta del enlace simbolico.");
		return;
	}
	else{
		buffer[r] = '\0';
		printf("El enlace apunta a %s. \n", buffer);
	}

	// Hacemos la copia del enlace simbolico en el destino
	int c = symlink(buffer, dest);
	if(c == -1){
		perror("Error realizando la copia del enlace simbolico.");
		return;
	}

	// Liberamos memoria
	free(buffer);
}

int main(int argc, char *argv[])
{
	struct stat s;
	int p;

	if (argc != 3)
	{
		fprintf(stderr, "Error introduciendo los parametros, deben ser origen y destino.");
		return 1;
	}

	// Hacemos el lstat para que actualice s y ver a que tipo de fichero apunta
	p = lstat(argv[1], &s);
	if (p == -1)
	{
		perror("Error haciendo lstat.");
		return 1;
	}

	// Miramos si es un fichero regular
	if (S_ISREG(s.st_mode))
	{
		copy_regular(argv[1], argv[2]);
	}
	// Miramos si es un enlace
	else if (S_ISLNK(s.st_mode))
	{
		copy_link(argv[1], argv[2]);
	}
	else{
		fprintf(stderr, "Fichero no valido para copia.");
		return 1;
	}

	return 0;
}
