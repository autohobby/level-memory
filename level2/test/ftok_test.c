#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// key_t ahm_path2key(const char* filename, const int id) {
//     return ftok(filename, id);
// }

int main() {
    printf("%d\n", ftok("/home/alpc32/tt", 0));
    // printf("%d\n", ftok("/hello/alpc33", 1));
    // printf("%d\n", ftok("/hello/alpc34", 2));
    // printf("%d\n", ftok("/hello/alpc35", 3));
    // printf("%d\n", ftok("/hello/alpc36", 4));
    // printf("%d\n", ftok("/hello/alpc37", 5));
    return 0;
}