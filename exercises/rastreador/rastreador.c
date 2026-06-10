#define _GNU_SOURCE
#include <stdio.h>    // Para printf, fprintf, perror, stdin, stdout
#include <stdlib.h>   // Para atoi, exit, malloc, free
#include <unistd.h>   // Proporciona getopt, read, write y close
#include <dirent.h>   // Para opendir y readdir
#include <sys/stat.h> // Para struct stat y las macros como S_ISREG (metadatos)
#include <fcntl.h>    // Para open
#include <string.h>

int main(int argc, char *argv[])
{
    int opt;
    int numBytes = -1;
    char *fichero_salida = NULL;
    FILE *salida = stdout; // Salida por defecto

    while ((opt = getopt(argc, argv, "n:o:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            numBytes = atoi(optarg);
            break;

        case 'o':
            fichero_salida = optarg;
            break;

        default:
            fprintf(stderr, "Uso: %s - n <numBytes> [ -o <outFileName> ]", argv[0]);
            return 1;
        }
    }
    // Miramos que numbBytes no tenga un error
    if (numBytes == -1)
    {
        fprintf(stderr, "Error: La opción -n es obligatoria.\n");
        fprintf(stderr, "Uso: %s -n <numBytes> [-o <outFileName>]\n", argv[0]);
        return 1;
    }

    if (fichero_salida != NULL) {
        salida = fopen(fichero_salida, "w");
        if (salida == NULL) {
            perror("Error al abrir el archivo de destino");
            return 1;
        }
    }

    DIR *dir;
    struct dirent *entrada_dir;

    // Abrimos el directorio actual
    dir = opendir(".");
    if (dir == NULL)
    {
        perror("Error al abrir el directorio");
        return 1;
    }

    // Readdir da un archivo en cada vuelta hasta que devuelva NULL
    while ((entrada_dir = readdir(dir)) != NULL)
    {
        // Ignoramos el directorio padre o actual
        if (strcmp(entrada_dir->d_name, "..") == 0 ||
            strcmp(entrada_dir->d_name, ".") == 0)
        {
            continue;
        }

        struct stat info;
        if (lstat(entrada_dir->d_name, &info) < 0)
        {
            perror("Error al hacer lstat");
            continue;
        }

        // Miramos si es un archivo regular
        if (!S_ISREG(info.st_mode))
        {
            continue;
        }

        // Leemos los datos
        int fd = open(entrada_dir->d_name, O_RDONLY);
        if (fd < 0) {
            perror("Error al abrir el archivo de lectura");
            continue;
        }

        // Reservamos memoria real para guardar los bytes
        char *nDatos = malloc(numBytes);
        if (nDatos == NULL) {
            perror("Error en malloc");
            close(fd);
            continue;
        }

        // Guardamos cuantos bytes se han leido realmente 
        int bytes_leidos = read(fd, nDatos, numBytes);
        if (bytes_leidos < 0) {
            perror("Error al leer el archivo");
        } else {
            fwrite(nDatos, 1, bytes_leidos, salida); 
            if (fichero_salida == NULL) {
                fputc('\n', salida);
            }
        }

        // Limpiamos todo antes de ir a por el siguiente archivo
        free(nDatos);
        close(fd);

        printf("Procesando: %s\n", entrada_dir->d_name);
    }

    if (salida != stdout) {
        fclose(salida);
    }

    closedir(dir);
    return 0;
}