#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ahm.h"

void sleep_ms(int ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms - ts.tv_sec * 1000) * 1000000;
  nanosleep(&ts, NULL);
}

static void test() {
    ahm_mcb temp;
    ahm_mcb* mcb = &temp;
    int i;
    if (ahm_open(mcb, "/hello/world", 1024*1024*32) == -1) {
        perror("ahm open");
        return;
    }
    for (i=0; i<1024; i+=4) {
        int* t = mcb->buf+i;
        *t = i*1000;
    }
    ahm_close(mcb);
}

static void test_e1array() {
    const char* filename = "/home/alpc32/tt";
    uint32_t size = 128*8;
    ahm_e1array* e1array = NULL;
    uint32_t remain;
    char cbuf[256];
    byte* buf;
    int i;
    int j;
    char ch = 'a';
    uint32_t index;
    uint32_t bi;

    e1array = ahm_open_e1array(filename, 1024, 24);
    // buf = ahm_e1array_tptr(e1array);
    printf("mcb->buf %p\n", e1array->mcb->buf);
    // printf("%p \n", buf);
    // printf("%d \n", remain);
    
    index = *(e1array->l);
    printf("%d %d\n", *(e1array->l), *(e1array->r));
    while (true) {
        while (index == *(e1array->r)) {
            sleep_ms(1);
        }
        bi = e1array->v[index%e1array->num];
        size = ahm_e1array_isize(e1array, index);
        ahm_e1array_cpy2buf(e1array, bi, cbuf, size);
        cbuf[size] = 0;
        printf("%s\n", cbuf);
        // printf("%d\n", size);
        index ++;
    }

    ahm_close(e1array->mcb);
    // ahm_remove(e1array->mcb);
    free(e1array);
}


int main() {
    // test();
    test_e1array();
    return 0;
}
