#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>   // Para S_IRUSR y S_IWUSR

int main(void)
{
    int fd1,i;
    char c;
    char buffer[6];
    ssize_t w, p;

    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    write(fd1, "00000", 5);
    for (i =1 ; i < 10; i++) {
        if (fork() == 0) {
            /* Child */
            sprintf(buffer, "%d", i*11111);
            //lseek(fd1, pos, SEEK_SET);
            //write(fd1, buffer, 5);
            // En vez de usar lo anterior usamos pwrite que dice donde hay que escribirlo directamente
            if((w = pwrite(fd1, buffer, 5, 10 * i - 5)) == -1){
                perror("Error escribiendo usando pwrite em el hijo.");
                return -1;
            }
            close(fd1);
            exit(0);
        } else {
            /* Parent */
            //lseek(fd1, 5, SEEK_CUR);
            // Usamos pwrite para escribir el patron intercalado del padre
            if((p = pwrite(fd1, "00000", 5, 10 * i)) == -1){
                perror("Error escribiendo con pwrite en el padre");
                return -1;
            }
        }
    }

	//wait for all childs to finish
    while (wait(NULL) != -1);

    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}
