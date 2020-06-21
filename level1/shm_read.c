#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// #define FILENAME "/ahm/ov"

int main(void){
    const char *filename = "/ahm/ov";
    key_t shm_key = ftok(filename, 0);
    int shm_size = getpagesize();
    int shm_id = shmget(shm_key, shm_size, 0644 | IPC_CREAT);

    if(shm_id == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    char *data = (char *)shmat(shm_id, NULL, 0);
    if(data == (void *)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    printf("data: %s\n", data);

    if(shmdt(data) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    if(shmctl(shm_id, IPC_RMID, NULL) == -1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
