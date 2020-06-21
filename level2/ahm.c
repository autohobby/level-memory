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
#include <pthread.h>

#include "ahm.h"

inline bool cas_uint32(uint32_t* ptr, uint32_t oldval, uint32_t newval) {
    return __sync_bool_compare_and_swap(ptr, oldval, newval);
}

key_t ahm_path2key(const char* filename, const int id) {
    return ftok(filename, id);
}

int ahm_create_shm(const key_t key, const int size) {
    int shm_id = shmget(key, size, 0644 | IPC_CREAT);
    return shm_id;
}

byte* ahm_get_shmptr(const int shmid) {
    byte* buf = (byte*)shmat(shmid, NULL, 0);
    return buf;
}

void ahm_shmdt(byte* buf) {
    shmdt(buf);
}

void ahm_ctl(const int shmid) {
    shmctl(shmid, IPC_RMID, NULL);
}

//////////////////////////////////////////////

/////////////////////////////////////////////

void ahm_get_mcb(const char* filename, ahm_mcb* mcb) {
    mcb->shmkey = ahm_path2key(filename, 0);
    mcb->shmid = ahm_create_shm(mcb->shmkey, mcb->shmsize);
    mcb->buf = ahm_get_shmptr(mcb->shmid);
}

int ahm_open(ahm_mcb* mcb, const char* filename, const int size) {
    mcb->shmsize = size;
    mcb->shmkey = ftok(filename, 0);
    if (mcb->shmkey==-1) {
        mcb->shmkey=3;
    }
    printf("%d %d\n", mcb->shmsize, mcb->shmkey);
    printf("%s\n", filename);
    mcb->shmid = shmget(mcb->shmkey, mcb->shmsize, 0644 | IPC_CREAT);
    if (mcb->shmid==-1) {
        perror("shmget");
        return -1;
    }
    mcb->buf = (byte*)shmat(mcb->shmid, NULL, 0);
    if (mcb->buf == (void *)-1) {
        perror("shmat");
        return -1;
    }
    return 1;
}

// ahm_mcb* ahm_open(const char* filename, const uint size) {
//     ahm_mcb temp;
//     temp.size = size;
//     ahm_mcb* mcb = &temp;
//     mcb->shmkey = ahm_path2key(filename, 0);
//     mcb->shmid = ahm_create_shm(mcb->shmkey, mcb->size);
//     mcb->buf = ahm_get_shmptr(mcb->shmid);
//     return mcb;
// }

// int open(const char* filename) {
//     return 0;
// }

void ahm_close(const ahm_mcb* mcb) {
    if (shmdt(mcb->buf) == -1) {
        perror("shmdt");
    }
}

void ahm_remove(const ahm_mcb* mcb) {
    // printf("id %d %p buf %p\n", mcb->shmid, &mcb->shmid, mcb->buf);
    if (shmctl((int)mcb->shmid, IPC_RMID, NULL)==-1) {
        perror("shmctl");
    }
}

/*
*/
bool ahm_open_f1array(ahm_f1array* f1array, const char *f,
                      const uint32_t onesize, const uint32_t num) {
    f1array->onesize = onesize;
    f1array->num = num;
    if (ahm_open(f1array->mcb, f, onesize*num+4) == -1) {
        perror("ahm open f1array");
        return 0;
    }
    f1array->top = (uint32_t*)f1array->mcb->buf;
    printf("ptr %p\n", f1array->mcb->buf);
    *f1array->top = 0;
    printf("hello\n");
    printf("%d\n", *f1array->top);
    printf("hello\n");
    return 1;
}

byte* ahm_f1array_tptr(ahm_f1array* f1array) {
    uint32_t i;
    // printf("temp\n");
    // printf("%d\n", *f1array->top);
    // printf("%d %d %d\n", *f1array->top, f1array->num, f1array->onesize);
    i = ((*f1array->top)%f1array->num)*f1array->onesize;
    i += 4;
    // printf("%d\n", i);
    return (f1array->mcb->buf+i);
}

bool ahm_f1array_check_index(ahm_f1array* f1array, uint32_t index) {
    return (*f1array->top)-index<f1array->num && (*f1array->top)>index;
}

byte* ahm_f1array_iptr(ahm_f1array* f1array, uint32_t index) {
    uint32_t i;
    if (!ahm_f1array_check_index(f1array, index)) {
        return NULL;
    }
    i = (index%f1array->num)*f1array->onesize;
    i += 4;
    return f1array->mcb->buf+i;
}

void ahm_f1array_push(ahm_f1array* ahm_f1array) {
    (*ahm_f1array->top) ++;
}


/*
*/
bool ahm_open_e1array(ahm_e1array* e1array, const char *f,
                      const uint32_t size, const uint32_t num) {
    int total_size = 0;
    e1array->num = num;
    total_size = 2 + 4*num + size;
    return ahm_open(&(e1array->mcb), f, total_size);
}

byte ahm_e1array_tptr(ahm_e1array* e1array, uint32_t* remain) {
    uint32_t buf_idx0 = e1array->v[*(e1array->r)];
    uint32_t buf_idx1 = e1array->v[*(e1array->l)];
    *remain = (buf_idx1+e1array->size)-buf_idx0;
    return e1array->mcb->buf+buf_idx0;
}

bool ahm_e1array_check_index(ahm_e1array* e1array, uint32_t index) {
    return *e1array->l<=index && *e1array->r>index;
}

byte ahm_e1array_iptr(ahm_e1array* e1array, uint32_t index) {
    uint32_t i;
    if (!ahm_e1array_check_index(e1array, index)) {
        return NULL;
    }
    i = e1array->v[index%e1array->num];
    return e1array->mcb->buf+i;
}

void ahm_e1array_push(ahm_e1array* ahm_e1array) {
    *ahm_e1array->r ++;
}

void ahm_e1array_memcpy(ahm_e1array* e1array, uint32_t i,
                        byte* buf, uint32_t size) {
    uint32_t remain = e1array->size - i;
    if (remain) {
        memcpy(e1array->mcb->buf+i, buf, size-remain);
        memcpy(e1array->mcb->buf, buf+size-remain, remain);
    } else {
        memcpy(e1array->mcb->buf+i, buf, size);
    }
}

void ahm_e1array_pushtop(ahm_e1array* e1array, byte* buf, uint32_t size) {
    uint32_t remain;
    if (e1array->size<size) {
        perror("size overload");
    }
    uint32_t buf_idx0 = e1array->v[*(e1array->r)];
    uint32_t buf_idx1 = e1array->v[*(e1array->l)];
    remain = (buf_idx1+e1array->size)-buf_idx0;
    remain -= size;
    while (remain<0) {
        remain += e1array->v[(*e1array->l)+1]-e1array->v[*e1array->l];
        *e1array->l ++;
    }
    ahm_e1array_memcpy(e1array, buf_idx0, buf, size);
}


void ahm_e1array_popbtm(ahm_e1array* e1array) {
    *e1array->l ++;
}
