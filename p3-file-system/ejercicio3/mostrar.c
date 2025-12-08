#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> // Funcion open

extern char *optarg;	// Para poder usar optarg de getopt
extern int optind;		// Para poder usar optind de getopt


int main(int argc, char *argv[])
{
	int N = 0, e = 0;
	int opt = 0;
	int fd = 0;

	//Miramos con el while y el switch si tenemos los argumentos y que hacemos con ellos
	while ((opt = getopt(argc, argv, "en:")) != -1)
	{
		switch (opt)
		{
			// Caso de argumento n
			case 'n':
				N = atoi(optarg);
			break;
			// Caso argumento e
			case 'e':
				e = 1;
			break;
			default:
				fprintf(stderr, "Error con el argumento dado.");
				return 1;
			break;
		}
	}

	// Abrimos el archivo
	if((fd = open(argv[optind], O_RDONLY)) == -1){
		perror("Error abriendo el archivo");
		return 1;
	}

	// Nos situamos donde toque
	if(e){
		lseek(fd, -N, SEEK_END);
	}
	else{
		lseek(fd, N, SEEK_SET);
	}

	// Leemos y escribimos
	char c;
	while((read(fd, &c, 1)) != 0){
		write(STDOUT_FILENO, &c, sizeof(char));	// STDOUT_FILENO es 1 para stdout
	}

	return 0;
}
