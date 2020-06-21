#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// #define FILENAME "/ahm/ov"

int main(int argc, char *argv[]){
    const char *filename = "/ahm/ov";
    if(argc < 2){
        fprintf(stderr, "usage: %s string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    strcpy(data, argv[1]);

    if(shmdt(data) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}
