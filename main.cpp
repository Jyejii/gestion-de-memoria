#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define SIZE 4096
#define SEM_NAME "/my_semaphore"

int main() {
    char *shared_memory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        sem_wait(sem); // Wait for parent to write
        printf("Child reads: %s\n", shared_memory);
        munmap(shared_memory, SIZE);
        sem_close(sem);
        sem_unlink(SEM_NAME);
        exit(EXIT_SUCCESS);
    } else {
        strcpy(shared_memory, "Hello, child process!");
        sem_post(sem); // Signal child that data is ready
        wait(NULL);
        munmap(shared_memory, SIZE);
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }

    return 0;
}


