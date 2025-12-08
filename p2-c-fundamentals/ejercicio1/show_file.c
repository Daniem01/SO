#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c,ret;
	char buffer[1024];

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	// El while va leyendo bit a bit el file
	while ((c = fread(buffer, 1, sizeof(buffer), file)) > 0) { // Aqui poner fread 
		/* Print byte to stdout */
		// Escribe en el buffer bit a bit todo usando c como tamaño, imprime con stdout
		ret = fwrite(buffer, 1, c, stdout);	// Aqui poner fwrite

		if (ret!=c){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}

	fclose(file);
	return 0;
}
