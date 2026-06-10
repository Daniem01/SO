#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

int run_command(char *cmd) {
    pid_t pid;
    struct sigaction sa;
    struct timeval t_inicial, t_final;
    struct rusage uso;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;

    // Ignoramos señales
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Capturamos tiempo
    gettimeofday(&t_inicial, NULL);

    pid = fork();

    if (pid == -1) {
        perror("Error en fork");
        return -1;
    }
    else if (pid == 0) {
        // Hacemos que vuelva a recibir las señales
        sa.sa_handler = SIG_DFL;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);

        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        perror("Error en exec");
        exit(EXIT_FAILURE);
    }
    else {
        printf("child process pid %d\n", pid);
        int estado;
        wait(&estado);
        // Si el hijo murio de forma violenta
        if(WIFSIGNALED(estado) == 1){

            printf("child with pid %d terminated by signal %d\n", pid, WTERMSIG(estado));
        }
    }

    // Capturamos tiempo y calculamos
    gettimeofday(&t_final, NULL);
    getrusage(RUSAGE_CHILDREN, &uso);
    double tiempo_real = (t_final.tv_sec - t_inicial.tv_sec) + 
                         (t_final.tv_usec - t_inicial.tv_usec) / 1000000.0;
    double tiempo_user = uso.ru_utime.tv_sec + 
                        uso.ru_utime.tv_usec / 1000000.0;
    double tiempo_sys = uso.ru_stime.tv_sec + 
                        uso.ru_stime.tv_usec / 1000000.0;
    printf("real: %3fs <-- puede fluctuar un poco.\n", tiempo_real);
    printf("user: %3fs <-- puede fluctuar un poco.\n", tiempo_user);
    printf("sys:  %3fs <-- puede fluctuar un poco.\n", tiempo_sys);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
        return 1;
    }

    run_command(argv[1]);
    return 0;
}