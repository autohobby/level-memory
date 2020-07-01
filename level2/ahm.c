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
    printf("ahm open begin");
    mcb->shmsize = size;
    mcb->shmkey = ftok(filename, 0);
    // if (mcb->shmkey==-1) {
    //     mcb->shmkey=3;
    // }
    mcb->shmid = shmget(mcb->shmkey, mcb->shmsize, 0644 | IPC_CREAT);
    printf("shmsize: %d shmkey: %d shmid: %d\n", mcb->shmsize, mcb->shmkey,
                                                 mcb->shmid);
    printf("filename: %s\n", filename);
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
// bool ahm_open_e1array(ahm_e1array* e1array, const char *f,
//                       const uint32_t size, const uint32_t num) {
//     int total_size = 0;
//     e1array->num = num;
//     total_size = 2*4 + 4*num + size;
//     return ahm_open(&(e1array->mcb), f, total_size);
// }

/*
     l  r  v      b
     4B 4B 4B*num 1B*size
init 0  0  0      na
l: bottom item index 
r: top item index(un push)
v: index vector
b: item buf
*/
ahm_e1array* ahm_open_e1array(const char *f, const uint32_t size,
                              const uint32_t num) {
    ahm_e1array* e1array = malloc(sizeof(ahm_e1array));
    e1array->mcb = malloc(sizeof(ahm_mcb));
    // item num
    e1array->num = num;
    // buf size
    e1array->size = size;
    int total_size = 4 + 4 + 4*num + size;

    printf("e1array total shm size: %d\n", total_size);
    printf("e1array file path: %s\n", f);

    if (ahm_open(e1array->mcb, f, total_size) == -1) {
        perror("ahm e1array error");
    }

    e1array->l = (uint32_t*)(e1array->mcb->buf);
    e1array->r = (uint32_t*)(e1array->mcb->buf+4);
    e1array->v = (uint32_t*)(e1array->mcb->buf+8);
    e1array->b = e1array->mcb->buf+8+4*(e1array->num);

    return e1array;
}

void ahm_e1array_init(ahm_e1array* e1array) {
    *(e1array->l) = 0;
    *(e1array->r) = 0;
    e1array->v[0] = 0;
    printf("e1array->b %p\n", e1array->b);
    printf("init e1array success\n");
}


// e1array top item buf index
static inline uint32_t e1_rbi(ahm_e1array* e1array) {
    return e1array->v[*(e1array->r)%e1array->num];
}

// e1array bottom item buf index
static inline uint32_t e1_lbi(ahm_e1array* e1array) {
    return e1array->v[*(e1array->l)%e1array->num];
}

// e1array i item buf index
static inline uint32_t e1_ibi(ahm_e1array* e1array, uint32_t i) {
    return e1array->v[i%e1array->num];
}

// static inline byte* _buf(ahm_e1array* e1array, uint32_t i) {
//     return e1array->b[i];
// }

static inline bool empty(ahm_e1array* e1array) {
    return *(e1array->l) == *(e1array->r);
}

// static inline void delete_l(ahm_e1array* e1array) {
// }

byte* ahm_e1array_tptr(ahm_e1array* e1array) {
    return e1array->b+e1_rbi(e1array);
}

inline byte* ahm_e1array_iptr(ahm_e1array* e1array, uint32_t index) {
    return e1array->b+e1_ibi(e1array, index);
}

uint32_t ahm_e1array_isize(ahm_e1array* e1array, uint32_t index) {
    uint32_t i, j;

    i = index%e1array->num;
    j = (index+1)%e1array->num;

    i = e1array->v[i];
    j = e1array->v[j];
    return (e1array->size+j-i)%e1array->size;
}

uint32_t ahm_e1array_remain(ahm_e1array* e1array) {
    if (empty(e1array)) {
        return e1array->size;
    }
    uint32_t remain = ((e1_lbi(e1array)+e1array->size)-e1_rbi(e1array)) % e1array->size;
    return remain;
    // return ((e1_lbi(e1array)+e1array->size)-e1_rbi(e1array)) % e1array->size;
}

inline bool ahm_e1array_checki(ahm_e1array* e1array, uint32_t index) {
    return *e1array->l<=index && index < *e1array->r;
}

void ahm_e1array_cpy2buf(ahm_e1array* e1array, uint32_t bi,
                        byte* buf, uint32_t size) {
    uint32_t remain = e1array->size - bi;
    if (remain < size) {
        memcpy(buf, e1array->b+bi, size-remain);
        memcpy(buf+size-remain, e1array->b, remain);
    } else {
        // printf("memcpy %p %p %d\n", e1array->b+i, buf, size);
        memcpy(buf, e1array->b+bi, size);
    }
}

void ahm_e1array_cpy2array(ahm_e1array* e1array, uint32_t bi,
                        byte* buf, uint32_t size) {
    int32_t remain = e1array->size - bi;
    printf("bi %d size %d remain %d\n", bi, size, remain);
    if (remain < size) {
        printf("< bi %d size-remain %d\n", bi, size-remain);
        memcpy(e1array->b+bi, buf, size-remain);
        printf("< size-remain %d remain %d\n", size-remain, remain);
        memcpy(e1array->b, buf+(size-remain), remain);
    } else {
        // printf("memcpy %p %p %d\n", e1array->b+i, buf, size);
        memcpy(e1array->b+bi, buf, size);
    }
}

// bool ahm_e1array_geti(ahm_e1array* e1array, uint32_t index, byte* buf, uint32_t* size) {
//     uint32_t i, j;
//     if (!ahm_e1array_checki(e1array, index)) {
//         return 0;
//     }

//     i = index%e1array->num;
//     j = (index+1)%e1array->num;

//     i = e1array->v[i];
//     j = e1array->v[j];
    
//     if (j>=i) {
//         size = j-i;
//         printf("memcpy %p %p %d\n", buf, e1array->b+i, size);
//         memcpy(buf, e1array->b+i, size);
//     } else {
//         size = (j+e1array->size-i) % e1array->size;
//         memcpy(buf, e1array->b+i, e1array->size-i);
//         memcpy(buf+e1array->size-i, e1array->b, j);
//     }
//     return 1;
// }

// void ahm_e1array_push(ahm_e1array* ahm_e1array) {
//     *ahm_e1array->r ++;
// }

// void ahm_e1array_memcpy(ahm_e1array* e1array, uint32_t i,
//                         byte* buf, uint32_t size) {
//     uint32_t remain = e1array->size - i;
//     if (remain < size) {
//         memcpy(e1array->b+i, buf, size-remain);
//         memcpy(e1array->b, buf+size-remain, remain);
//     } else {
//         printf("memcpy %p %p %d\n", e1array->b+i, buf, size);
//         memcpy(e1array->b+i, buf, size);
//     }
// }

void ahm_e1array_pushtop(ahm_e1array* e1array, byte* buf, uint32_t size) {
    int32_t remain;
    int32_t rbi = e1_rbi(e1array);

    if (e1array->size<size) {
        perror("size overload");
    }

    remain = ahm_e1array_remain(e1array);
    printf("remain: %d\n", remain);

    printf("L: %d R: %d\n", *(e1array->l), *(e1array->r));
    printf("lbi: %d rbi: %d\n", e1_lbi(e1array), e1_rbi(e1array));
    printf("remain: %d size: %d\n", remain, size);

    remain -= size;
    while (remain<0) {
        remain += ahm_e1array_isize(e1array, *(e1array->l));
        (*e1array->l) ++;
    }
    // printf("mem %p %d %p %d\n", e1array, buf_idx0, buf, size);
    // ahm_e1array_memcpy(e1array, buf_idx0, buf, size);
    printf("bi: %d\n", rbi);

    ahm_e1array_cpy2array(e1array, rbi, buf, size);

    printf("memcp over\n");

    if ( ((*e1array->r)+1)%e1array->num == (*e1array->l)%e1array->num ) {
        (*e1array->l) ++;
    }

    (*e1array->r) ++;
    e1array->v[(*e1array->r)%e1array->num] = (rbi+size)%e1array->size;

    printf("L: %d R: %d\n", *(e1array->l), *(e1array->r));
    printf("lbi: %d rbi: %d\n", e1_lbi(e1array), e1_rbi(e1array));
    // printf("idx %d\n", buf_idx0);
    // printf("remain %d\n", remain);
    // printf("%d %d\n", *e1array->l, *e1array->r);
    // printf("%d %d\n", e1array->v[*e1array->l], e1array->v[*e1array->l]);
}

// void ahm_e1array_popbtm(ahm_e1array* e1array) {
//     *e1array->l ++;
// }
