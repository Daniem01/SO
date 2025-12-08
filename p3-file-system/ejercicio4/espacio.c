#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>	// Para struct stat, lstat y macros S_ISREG, S_ISLNK, etc.
#include <sys/types.h>
#include <dirent.h>		// Para poder usar DIR
#include <string.h>		// Para usar strcmp

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
	struct stat st;
	int p = 0;

	// Usamos la funcion lstat
	if((p = lstat(fname, &st)) == -1){
		perror("Error ejecutando la función lstat");
		return -1;
	}
	// Sumamos los bloques y comprobamos si es directorio
	*blocks += st.st_blocks;
	if(S_ISDIR(st.st_mode)){
		if((p = get_size_dir(fname, blocks)) == -1){
			perror("Error obteniendo tamaño de directorio");
			return -1;
		}
	}

	return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
	DIR *direction;
	char path[512];
	int s;

	// Abrimos el directorio
	direction = opendir(dname);
	if(direction == NULL){
		perror("Error abriendo el directorio");
		return -1;
	}

	// Leemos lo que hay dentro del directorio
	struct dirent *dir;
	while((dir = readdir(direction)) != NULL){
		// Si es el directiorio . o ..
		if(strcmp(dir->d_name, ".") == 0|| strcmp(dir->d_name, "..") == 0){
			continue;
		}

		// Escribimos la ruta en path
		sprintf(path, "%s/%s", dname, dir->d_name);

		// LLamamos a get_size
		if((s = get_size(path, blocks)) == -1){
			perror("Error obteniendo el tamaño del directorio");
			return -1;
		}
	}

	// Cerramos el directorio
	if(closedir(direction) == -1){
		perror("Error cerrando el directorio");
		return -1;
	}

	return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
	// Procesamos cada archivo
	for(int i  = 1; i < argc; i++){
		size_t bloques = 0;
		if(get_size(argv[i], &bloques) == -1){
			perror("Error obteniendo el tamaño");
			return -1;
		}

		printf("%luK %s\n", bloques/2, argv[i]);
	}

	return 0;
}
