#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> // Para el waitpid

extern char *optarg; // Para poder usar optarg de getopt

pid_t launch_command(char **argv)
{
    pid_t pid;
    pid = fork();

    int s = 0;

    // Vemos si somos el hijo
    if (pid == 0)
    {
        if ((s = execvp(argv[0], argv) == -1))
        {
            perror("Error realizando execvp.");
            exit(EXIT_FAILURE);
        }
        return 0;
    }
    // Vemos is somos el padre
    else if (pid > 0)
    {
        return pid;
    }
    // Error
    else
    {
        fprintf(stderr, "Error al hacer fork.");
        return -1;
    }
}

char **parse_command(const char *cmd, int *argc)
{
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len;
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start))
        start++; // Skip leading spaces

    while (*start)
    {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1)
        { // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL)
            {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end))
            end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0'; // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start))
            start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc) = arg_count; // Return argc

    return argv;
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

    // Miramos que accion realizamos
    while ((opt = getopt(argc, argv, "x:s:")) != -1)
    {
        switch (opt)
        {
        case 'x':
            cmd_argv = parse_command(optarg, &cmd_argc);
            pid = launch_command(cmd_argv);
            waitpid(pid, NULL, 0);
            // Limpiamos la memoria
            for (i = 0; cmd_argv[i] != NULL; i++)
            {
                free(cmd_argv[i]);
            }
            free(cmd_argv);
            break;
        case 's':
            if ((fp = fopen(optarg, "r")) == NULL)
            {
                perror("Error abriendo el fichero");
                return EXIT_FAILURE;
            }
            // Leer línea a línea
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                // Parseamos la línea leída
                cmd_argv = parse_command(line, &cmd_argc);

                // Si la línea está vacía, saltamos
                if (cmd_argv[0] == NULL)
                {
                    free(cmd_argv);
                    continue;
                }

                // Lanzamos el comando
                pid = launch_command(cmd_argv);

                waitpid(pid, NULL, 0);

                // Limpiamos la memoria
                for (i = 0; cmd_argv[i] != NULL; i++)
                {
                    free(cmd_argv[i]);
                }
                free(cmd_argv);
            }

            // Cerramos el fichero
            fclose(fp);
            break;
        // Por defecto tratamos si hay error
        default:
            fprintf(stderr, "Error trabajando el argumento.");
        }
    }

    return EXIT_SUCCESS;
}