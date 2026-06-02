#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> // IMPRESCINDIBLE para el wait/waitpid

extern char *optarg; // Para poder usar optarg de getopt

pid_t launch_command(char **argv)
{
    pid_t pid;
    
    // fork() clona el proceso actual
    pid = fork();

    int s = 0;

    // FLUJO DEL HIJO
    if (pid == 0)
    {
        // Las funciones exec REEMPLAZAN la imagen de memoria del proceso 
        // Si tienen éxito el código que haya debajo NUNCA se ejecuta
        // execvp busca el ejecutable en el PATH. Necesita el nombre (argv) y el array completo (argv).
        // El array argv DEBE terminar en NULL
        if (execvp(argv[0], argv) == -1) {
            perror("Error realizando execvp.");
            exit(EXIT_FAILURE);
        }
        return 0;
    }
    // FLUJO DEL PADRE
    else if (pid > 0)
    {
        // El padre simplemente retorna el PID del hijo que acaba de crear.
        return pid;
    }
    // FLUJO DE ERROR
    else
    {
        perror("Error al hacer fork.");
        return -1;
    }
}

int main(int argc, char *argv[])
{
    char **cmd_argv;
    int cmd_argc;
    int i;
    int opt;
    pid_t pid;
    FILE *fp;
    char line[256];

    // PROCESAMIENTO DE ARGUMENTOS CON GETOPT
    // Los dos puntos indican que requieren un argumento extra
    // optarg es una variable global de getopt que guarda ese argumento extra
    while ((opt = getopt(argc, argv, "x:s:")) != -1)
    {
        switch (opt)
        {
        case 'x':
            cmd_argv = parse_command(optarg, &cmd_argc);
            pid = launch_command(cmd_argv);
            
            // ESPERA DEL PROCESO HIJO 
            // waitpid espera específicamente al PID que le pasamos.
            // Pasamos NULL porque no nos importa cómo murio
            // Lo logico seria pasar una variable &status y usar WIFEXITED(status) y WEXITSTATUS(status).
            waitpid(pid, NULL, 0);

            // LIMPIEZA DE MEMORIA
            // Como parse_command usa malloc internamente hay que liberar cada string y el array
            for (i = 0; cmd_argv[i] != NULL; i++)
            {
                free(cmd_argv[i]);
            }
            free(cmd_argv);
            break;

        case 's':
            // APERTURA DE FICHEROS
            // fopen con "r" abre en modo lectura
            if ((fp = fopen(optarg, "r")) == NULL)
            {
                perror("Error abriendo el fichero");
                return EXIT_FAILURE;
            }
            
            // LECTURA LÍNEA A LÍNEA
            // Usar fgets es la forma más segura de leer líneas enteras
            // Leemos como máximo sizeof(line) para evitar desbordamientos de memoria
            // Devuelve NULL cuando llega al final del fichero
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                cmd_argv = parse_command(line, &cmd_argc);
                if (cmd_argv == NULL)
                {
                    free(cmd_argv);
                    continue; 
                }
                // Lanzamos el proceso hijo
                pid = launch_command(cmd_argv);

                // Esperamos a que termine antes de lanzar el siguiente
                waitpid(pid, NULL, 0);

                // Limpiamos la memoria
                for (i = 0; cmd_argv[i] != NULL; i++)
                {
                    free(cmd_argv[i]);
                }
                free(cmd_argv);
            }

            // CIERRE DEl FICHERO
            fclose(fp);
            break;

        // Si el usuario mete una opción que no es -x ni -s
        default:
            fprintf(stderr, "Error trabajando el argumento.");
        }
    }

    return EXIT_SUCCESS;
}