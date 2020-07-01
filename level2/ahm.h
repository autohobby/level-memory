#ifndef _AHM_H_
#define _AHM_H_

#include <stdint.h>
#include <stdbool.h>

// typedef enum {
//     A,
//     B
// } ahm_v;

typedef unsigned char byte;
typedef unsigned int uint;
// typedef unsigned int uint;
typedef struct ahm_mcb ahm_mcb;
struct ahm_mcb {
    char* filename;
    int shmid;
    int shmsize;
    key_t shmkey;
    byte* buf;
};


// type 0 fixed len
typedef struct ahm_f1array ahm_f1array;
struct ahm_f1array {
    ahm_mcb* mcb;
    uint32_t *top;
    uint32_t onesize;
    uint32_t num;
};

struct ahm_frarray {
    ahm_mcb mcb;
    uint32_t* top;
    uint32_t onesize;
    uint32_t num;
};

struct ahm_fnarray {
    ahm_mcb mcb;
    uint32_t top;
    uint32_t onesize;
    uint32_t num;
};

// type 1 not fixed len
typedef struct ahm_e1array ahm_e1array;

struct ahm_e1array {
    ahm_mcb* mcb;
    uint32_t* l;
    uint32_t* r;
    uint32_t* v;
    byte* b;
    uint32_t num;
    uint32_t size;
};

#ifdef __cplusplus

extern "C" {

#endif

int ahm_open(ahm_mcb* mcb, const char* filename, const int size);
void ahm_close(const ahm_mcb* mcb);
void ahm_remove(const ahm_mcb* mcb);

bool ahm_open_f1array(ahm_f1array* f1array, const char *f,
                      const uint32_t onesize, const uint32_t num);

byte* ahm_f1array_tptr(ahm_f1array* f1array);

bool ahm_f1array_check_index(ahm_f1array* f1array, uint32_t index);

byte* ahm_f1array_iptr(ahm_f1array* f1array, uint32_t index);

void ahm_f1array_push(ahm_f1array* ahm_f1array);

byte* ahm_e1array_tptr(ahm_e1array* e1array);

ahm_e1array* ahm_open_e1array(const char *f, const uint32_t size,
                              const uint32_t num);

void ahm_e1array_init(ahm_e1array* e1array);

bool ahm_e1array_check_index(ahm_e1array* e1array, uint32_t index);

inline byte* ahm_e1array_iptr(ahm_e1array* e1array, uint32_t index);

void ahm_e1array_pushtop(ahm_e1array* e1array, byte* buf, uint32_t size);

void ahm_e1array_cpy2buf(ahm_e1array* e1array, uint32_t bi,
                        byte* buf, uint32_t size);

uint32_t ahm_e1array_isize(ahm_e1array* e1array, uint32_t index);

void ahm_e1array_push(ahm_e1array* ahm_e1array);

#ifdef __cplusplus

}

#endif



#endif
