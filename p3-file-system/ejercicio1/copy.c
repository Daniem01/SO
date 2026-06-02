#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   // Para los flags de open() (O_RDONLY, O_CREAT, etc.)
#include <unistd.h>  // Para read(), write() y close()

void copy(int fdo, int fdd)
{
    // El buffer no necesita ser enorme. 512 o 1024 está bien.
    char buffer; 
    ssize_t r = 0;

    // read() devuelve el número de bytes leídos
    // Si devuelve 0, hemos llegado al EOF (Fin de Fichero)
    // Si devuelve < 0, hubo un error.
    while ((r = read(fdo, buffer, 512)) > 0) 
    {
        // En write() no se pone el tamaño total del buffer
        // Hay que poner los bytes que has leído ('r')
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
        fprintf(stderr, "Uso correcto: %s <fichero_origen> <fichero_destino>\n", argv);
        return 1;
    }

    // APERTURA MODO LECTURA
    // argv[5] es el primer parámetro que pasa el usuario.
    int fo = open(argv[5], O_RDONLY);
    if (fo == -1) // Siempre hay que comprobar si open() falla devolviendo -1
    {
        perror("Error en la apertura del archivo origen.");
        return 1;
    }

    // APERTURA MODO ESCRITURA
    // O_WRONLY: Solo escritura.
    // O_CREAT: Si el fichero no existe, lo crea.
    // O_TRUNC: Si el fichero ya existe, borra su contenido dejándolo a tamaño 0.
    // 0644: Permisos si se crea el fichero
    int fd = open(argv[6], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("Error abriendo el archivo destino.");
        return 1;
    }

    copy(fo, fd);
    close(fd);
    close(fo);

    return 0;
}